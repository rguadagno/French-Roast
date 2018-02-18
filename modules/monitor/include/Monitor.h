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
#include "MethodStats.h"
#include "StackReport.h"
#include "ClassDetail.h"
#include "JammedReport.h"
#include "Signal.h"
#include "SignalReport.h"
#include "TimerReport.h"
#include "Command.h"
#include "HeapEvent.h"
#include "HeapMonitor.h"
#include "Connector.h"

namespace frenchroast { namespace monitor {
  void transmit_lines(const std::string& fileName, const std::string& ipport, frenchroast::network::Connector<>&);
  void transmit_lines(const std::vector<std::string>&, const std::string& ipport, frenchroast::network::Connector<>&);
  
    
    template
    <typename T>
      class Monitor : public network::Listener {
      using servicer = void (Monitor<T>::*)(const std::vector<std::string>&);
      
      class Executer {
        public:
          Executer() : _ptr(&Monitor<T>::service_ignore) {}
          Executer(servicer ptr) : _ptr(ptr) {}
          servicer _ptr;
      };


	T&                                                                             _handler;
        network::Connector<>                                                          _conn;
        std::unordered_map<std::string, TimerReport>                                  _timers;
        std::unordered_map<std::string, SignalReport>                                 _signals;
        std::unordered_map<std::string, MethodStats>                                  _method_counters;
        std::unordered_map<std::string, JammedReport>                                 _jammedReports;
        HeapMonitor<T>                                                                _heapMonitor;
        std::string                                                                   _opcodeFile;
        std::unordered_map<std::string, std::string>                                  _clients;
        boost::lockfree::spsc_queue<std::string, boost::lockfree::fixed_sized<true>>  _iq{15000};
        std::unordered_map<char, Executer>                                            _commands;
        const int IP_PORT     = 0;
       
        const int HOST_NAME   = 2;
        const int PID         = 3;
        const int MSG         = 1;

    public:
    Monitor(T& handler, const std::string& opcodeFile) : _handler(handler), _heapMonitor(handler),_opcodeFile(opcodeFile)
        {
          _commands[command::SIGNAL]           = Executer{&Monitor<T>::service_signal};
          _commands[command::SIGNAL_TIMER]     = Executer{&Monitor<T>::service_timer};
          _commands[command::TRAFFIC]          = Executer{&Monitor<T>::service_traffic};
          _commands[command::JAMMED]           = Executer{&Monitor<T>::service_jammed};
          _commands[command::LOADED]           = Executer{&Monitor<T>::service_loaded};
          _commands[command::READY]            = Executer{&Monitor<T>::service_ready};
          _commands[command::CONNECTED]        = Executer{&Monitor<T>::service_connected};
          _commands[command::UNLOADED]         = Executer{&Monitor<T>::service_unloaded};
          _commands[command::ACK_PROFILER_ON]  = Executer{&Monitor<T>::service_ack_profiler_on};
          _commands[command::ACK_PROFILER_OFF] = Executer{&Monitor<T>::service_ack_profiler_off};
          _commands[command::TRANSMIT_OPCODES] = Executer{&Monitor<T>::service_transmit_opcodes};
          _commands[command::TRANSMIT_HOOKS]   = Executer{&Monitor<T>::service_transmit_hooks};
          _commands[command::HEAP_EVENT]       = Executer{&Monitor<T>::service_heap_event};
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
              (this->*_commands[items[MSG][0]]._ptr)(items);
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
          }
        }

        void service_timer(const std::vector<std::string>& parts)
        {
          TimerReport rpt{};
          parts[MSG] >> rpt;

          if (rpt.direction() == TimerReport::Direction::Exit) {
            _handler.signal_timed(_timers[rpt.key()] += rpt);
          }
          if (rpt.direction() == TimerReport::Direction::Enter) {
            _timers[rpt.key()] = rpt;
          }
        }

        void service_heap_event(const std::vector<std::string>& parts)
        {
          _heapMonitor.process(parts[MSG] >> HeapEvent{});
        }

        void service_signal(const std::vector<std::string>& parts)
        {
          Signal sig{};
          parts[MSG] >> sig;
          _handler.signal(_signals[sig.key()] += sig);
        }

        void service_traffic(const std::vector<std::string>& parts)
        {
          _handler.traffic( construct_traffic(parts[MSG], _method_counters));
        }

        void service_jammed(const std::vector<std::string>& parts)
        {
          _handler.jammed( _jammedReports[parts[MSG]] += (parts[MSG] >> JammedReport{}) );
        }
        
        void service_loaded(const std::vector<std::string>& parts)
        {
          std::vector<ClassDetail> details;
          _handler.class_watch( parts[MSG] >> details);
        }

        void service_ready(const std::vector<std::string>& parts)
        {
          _clients[parts[HOST_NAME] + parts[PID]] = parts[IP_PORT];
          _handler.ready(parts[HOST_NAME], parts[PID]);
        }

        void service_connected(const std::vector<std::string>& parts)
        {
          _handler.connected(parts[1], "");
        }
        
        void service_unloaded(const std::vector<std::string>& parts)
        {
          _handler.unloaded(parts[HOST_NAME], parts[PID]);
        }
        
        void service_ack_profiler_off(const std::vector<std::string>& parts)
        {
          
          _handler.ack_profiler_off(split(parts[MSG], "<host-name>")[0], split(parts[MSG], "<host-name>")[1]);
        }

        void service_ack_profiler_on(const std::vector<std::string>& parts)
        {
          _handler.ack_profiler_on(split(parts[MSG], "<host-name>")[0], split(parts[MSG], "<host-name>")[1]);
        }
        
        void service_transmit_opcodes(const std::vector<std::string>& parts)
        {
          transmit_lines(_opcodeFile, parts[IP_PORT], _conn);
        }

        void service_transmit_hooks(const std::vector<std::string>& parts)
        {
          _handler.request_hooks(parts[IP_PORT]);
        }

        void service_ignore(const std::vector<std::string>&)
        {
          
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
          _timers.clear();
          _signals.clear();
          _method_counters.clear();
          _jammedReports.clear();
          _heapMonitor.reset();
        }
        
    };
  }
}
#endif
