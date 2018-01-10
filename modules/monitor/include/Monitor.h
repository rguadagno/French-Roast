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
#include <sstream>
#include <boost/lockfree/spsc_queue.hpp>
#include "MonitorUtil.h"
#include "Listener.h"
#include "Util.h"
#include "StackTrace.h"
#include "MarkerField.h"
#include "MethodStats.h"
#include "StackReport.h"
#include "ClassDetail.h"
#include "JammedReport.h"

namespace frenchroast { namespace monitor {

    struct time_holder {
      long      _elapsed;
      long long _last;
    };
   
    template
    <typename T>
      class Monitor : public network::Listener {
	T&                                                                             _handler;
        network::Connector<>                                                          _conn;
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

        void message(const std::string& msg)
        {
          std::string nmsg = msg.substr(msg.find_first_of("~")+1);
          std::string ipport = msg.substr(0,msg.find_first_of("~")+1);
          for(auto& mitem : frenchroast::split(nmsg,"<end>")) {
            while(!_iq.push(ipport + mitem));
           
            }
        }
        
        void mhandler()
        {
          std::string msg;
          while(1) {
          while(_iq.pop(msg)) {
            std::vector<std::string> items = frenchroast::split(msg,"~");
            if (items[MSG_TYPE] == "signaltimer") {
              service_timer(items[DIRECTION], items[DESCRIPTOR], items[THREAD_NAME], items[TIME]);
            }
            if (items[MSG_TYPE] == "signal") {
              service_signal(items[MSG], items[SIGNAL_THREAD_NAME], items[PARAMS], items[MARKER], items[STACK]);
            }

            if (items[MSG_TYPE] == "traffic") {
              _handler.traffic( construct_traffic(items[MSG], _method_counters));
            }
            
            if (items[MSG_TYPE] == "jammed") {
              _handler.jammed( _jammedReports[items[MSG]] += (items[MSG] >> JammedReport{}) );
            }

            if (items[MSG_TYPE] == "loaded") {
              std::vector<ClassDetail> details;
              _handler.class_watch( items[MSG] >> details);
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

        void service_timer(const std::string& direction, const std::string& descriptor, const std::string& thread_name, const std::string& time)
        {
          if (direction == "exit") {
            std::string key = descriptor + thread_name;
            int elapsed = std::stoll(time) - _timed_signals[key]._last;
            _timed_signals[key]._elapsed += elapsed;
            _handler.signal_timed( descriptor_to_string(descriptor.substr(1)), thread_name , _timed_signals[key]._elapsed, elapsed);
          }
            if (direction == "enter") {
              std::string key = descriptor + thread_name;
              _timed_signals[key]._last = std::stoll(time);
            }
        }
        
        void service_signal(const std::string& msg, const std::string& thread_name, const std::string& params, const std::string& marker, const std::string& stack)
        {
          const std::string key = thread_name + msg;
          const std::string subkey = params + marker;
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
          std::string desc = descriptor_to_string(msg.substr(1));
          for(auto& x : frenchroast::split(frenchroast::split(frenchroast::split(desc,")")[0], "(")[1], ",")) {
            argHeaders.push_back(x);
          }
              
          std::string skey = "";
          std::vector<std::string> sframes;
  
          for(auto& x : frenchroast::split(stack, "%")) {
            if(x.find("::") != std::string::npos) {
              skey.append(x);
              sframes.push_back(descriptor_to_string(x.substr(1)));
            }
          }
          if(_stacks[thread_name + desc].count(skey) == 0) {
            _stacks[thread_name + desc][skey] = StackReport(sframes);
          }
          else {
            ++_stacks[thread_name + desc][skey];
          }
  
          _handler.signal(desc , thread_name  , ++_signals[thread_name  + msg], argHeaders, _instanceHeaders[key], _markers[key][subkey], _stacks[thread_name + desc]);
         
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
