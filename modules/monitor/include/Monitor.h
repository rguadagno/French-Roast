// copyright (c) 2016 Richard Guadagno
// contact: rrguadagno@gmail.com
//
// This file is part of French-Roast
//
//    French-Roast is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    French-Roast is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with French-Roast.  If not, see <http://www.gnu.org/licenses/>.
//

#ifndef FRMON_H
#define FRMON_H
#include <unordered_map>
#include <algorithm>
#include <thread>
#include <boost/lockfree/spsc_queue.hpp>
#include "Connector.h"
#include "Listener.h"
#include "Util.h"
#include "StackTrace.h"
#include "MarkerField.h"
#include "MethodStats.h"
#include "StackReport.h"
#include "ClassDetail.h"
#include "JammedReport.h"

namespace frenchroast { namespace monitor {

    
    std::string              translate_descriptor(const std::string& name, int* = nullptr);
    std::vector<StackTrace>  construct_traffic(const std::string& msg, std::unordered_map<std::string, MethodStats>& counters);
    JammedReport&            process_jammed(const std::string& monitor, const std::string& waiter, const std::string& owner,
                                            std::unordered_map<std::string, JammedReport>& jcount);
                                            
    std::vector<ClassDetail> construct_class_details(const std::string& msg);
    void transmit_lines(const std::string& fileName, const std::string& ipport, frenchroast::network::Connector&);
    void transmit_lines(const std::vector<std::string>&, const std::string& ipport, frenchroast::network::Connector&);

    struct time_holder {
      long      _elapsed;
      long long _last;
    };

   
    template
    <typename T>
      class Monitor : public network::Listener {
	T&                                                                             _handler;
        network::Connector                                                            _conn;
        std::unordered_map<std::string, time_holder>                                  _timed_signals;
        std::unordered_map<std::string, int>                                          _signals;
        std::unordered_map<std::string, MethodStats>                                  _method_counters;
        std::unordered_map<std::string, JammedReport>                                 _jammedReports;
        std::unordered_map<std::string, std::unordered_map<std::string, MarkerField>> _markers;
        std::unordered_map<std::string, std::vector<std::string>>                     _instanceHeaders;
        std::unordered_map<std::string, std::unordered_map<std::string, StackReport>> _stacks;
        std::string                                                                   _opcodeFile;
        std::unordered_map<std::string, std::string>                                  _clients;
        boost::lockfree::spsc_queue<std::string, boost::lockfree::fixed_sized<true>>  _iq{15000};

       const int IP_PORT     = 0;
       
       const int HOST_NAME   = 2;
       const int PID         = 3;
        
	const int MSG_TYPE    = 1;
	const int MSG         = 2;
       const int SIGNAL_THREAD_NAME = 3;

       const int MARKER      = 4;
       const int PARAMS      = 5;
       const int STACK       = 6;

	const int TIME        = 2;
	const int DIRECTION   = 3;
	const int DESCRIPTOR  = 4;
	const int THREAD_NAME = 5;

        const int MONITOR         = 2;
        const int WAITER          = 3;
        const int OWNER           = 4;
        
    public:
    Monitor(T& handler, const std::string& opcodeFile) : _handler(handler), _opcodeFile(opcodeFile)
        { 
        }

        MarkerField build_marker( std::string str)
        {
          MarkerField mf{str};

          for(auto& x: frenchroast::split(frenchroast::split(str, ")")[0].substr(1),",")) {
            mf._arg_items.push_back(x);
          }
                  
          for(auto& x: frenchroast::split(frenchroast::split(str, ")")[1],";")) {
            if(x.find(":") != std::string::npos) {
               mf._instance_items.push_back(frenchroast::split(x,":")[1]);
            }
          }

          return mf;
        }

        std::vector<std::string> build_instance_headers(const std::string& subkey)
        {
          std::vector<std::string> rv;
          for(auto& x: frenchroast::split(frenchroast::split(subkey, ")")[1],";")) {
           if(x.find(":") != std::string::npos) {
            rv.push_back(frenchroast::split(x,":")[0]);
           }
          }
          return rv;
        }
        
        void message(const std::string& msg)
        {
          while(!_iq.push(msg));
        }
        
        void mhandler()
        {
          std::string msg;
          while(1) {
          while(_iq.pop(msg)) {
            std::vector<std::string> items = frenchroast::split(msg,"~");
            if (items[MSG_TYPE] == "signaltimer") {
              if (items[DIRECTION] == "exit") {
                std::string key = items[DESCRIPTOR] + items[THREAD_NAME];
                int elapsed = std::stoll(items[TIME]) - _timed_signals[key]._last;
                _timed_signals[key]._elapsed += elapsed;
                _handler.signal_timed( translate_descriptor(items[DESCRIPTOR].substr(1)), items[THREAD_NAME] , _timed_signals[key]._elapsed, elapsed);
              }
              if (items[DIRECTION] == "enter") {
                std::string key = items[DESCRIPTOR] + items[THREAD_NAME];
                _timed_signals[key]._last = std::stoll(items[TIME]);
              }
            }
            if (items[MSG_TYPE] == "signal") {
              const std::string thread_name  = items[SIGNAL_THREAD_NAME];
              const std::string key = thread_name + items[MSG];
              const std::string subkey = items[PARAMS] + items[MARKER];
                if(_markers.count(key) == 0 || _markers[key].count(subkey) == 0) {
                _markers[key][subkey] = build_marker(subkey);
              }
              else {
                ++_markers[key][subkey];
              }

              if(_instanceHeaders.count(key) == 0) {
                _instanceHeaders[key] = build_instance_headers(subkey);
              }

              std::vector<std::string> argHeaders;
              std::string desc = translate_descriptor(items[MSG].substr(1));
              for(auto& x : frenchroast::split(frenchroast::split(frenchroast::split(desc,")")[0], "(")[1], ",")) {
                argHeaders.push_back(x);
              }
              
              std::string skey = "";
              std::vector<std::string> sframes;
  
                for(auto& x : frenchroast::split(items[STACK], "%")) {
                if(x.find("::") != std::string::npos) {
                  skey.append(x);
                  sframes.push_back(translate_descriptor(x.substr(1)));
                }
              }
              if(_stacks[thread_name + desc].count(skey) == 0) {
                _stacks[thread_name + desc][skey] = StackReport(sframes);
              }
              else {
                ++_stacks[thread_name + desc][skey];
              }
  
              _handler.signal(desc , thread_name  , ++_signals[thread_name  + items[MSG]], argHeaders, _instanceHeaders[key], _markers[key][subkey], _stacks[thread_name + desc]);

              }

            if (items[MSG_TYPE] == "traffic") {
              _handler.traffic( construct_traffic(items[MSG], _method_counters));
            }
            if (items[MSG_TYPE] == "jammed") {
              _handler.jammed( process_jammed(items[MONITOR], items[WAITER], items[OWNER], _jammedReports));
            }

            if (items[MSG_TYPE] == "loaded") {
              _handler.class_watch( construct_class_details(items[MSG]));
            }

            if (items[MSG_TYPE] == "ready") {
            _clients[items[HOST_NAME] + items[PID]] = items[IP_PORT];
	    _handler.ready(items[HOST_NAME], items[PID]);
            }
          
            if (items[MSG_TYPE] == "connected") {
              _handler.connected(items[1], "");
            }
          
            if (items[MSG_TYPE] == "unloaded") {
              _handler.unloaded(items[2], items[3]);
            }
            if(items[MSG_TYPE] == "transmit-opcodes") {
              transmit_lines(_opcodeFile, items[IP_PORT], _conn);
            }
            if(items[MSG_TYPE] == "transmit-hooks") {
              _handler.request_hooks(items[IP_PORT]);
            }
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            }
        }

        void send_hooks(const std::vector<std::string>& hooks, const std::string& ipport)
        {
          transmit_lines(hooks, ipport, _conn);
        }

        void init_receiver(const std::string& ipAddr, int port)
        {
          using MonitorMember = void (Monitor<T>::*)();
          MonitorMember ptr = &Monitor<T>::mhandler;
          std::thread t1{[&](){(this->*ptr)();}};
          t1.detach();
          _conn.wait_for_client_connection(ipAddr, port, this);
        }

        void turn_on_profiler(const std::string& hostname_pid)
        {
          _conn.send_message(_clients[hostname_pid], "turn_on_profiler");
        }

        void turn_off_profiler(const std::string& hostname_pid)
        {
          std::cout << "turnoff: " << hostname_pid << std::endl;
          _conn.send_message(_clients[hostname_pid], "turn_off_profiler");
        }

        void start_watch_loading()
        {
          _conn.send_message("watch_loading");
        }

        void stop_watch_loading()
        {
          _conn.send_message("stop_watch_loading");
        }

            
        void watch_traffic(const int interval_millis)
        {
          _conn.send_message("watch_traffic~" + std::to_string(interval_millis));
        }

        std::vector<MethodStats> stop_watch_traffic()
          {
            _conn.send_message("stop_watch_traffic~" );
            std::vector<MethodStats> rv;
            for(auto& x : _method_counters) {
              rv.push_back(x.second);
            }
            std::sort(rv.begin(), rv.end());
            return rv;
          }

        void reset()
        {
          _timed_signals.clear();
          _signals.clear();
          _method_counters.clear();
          _jammedReports.clear();
          _markers.clear();
          _stacks.clear();
        }
        
    };
  }
}
#endif
