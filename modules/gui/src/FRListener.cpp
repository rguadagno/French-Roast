// copyright (c) 2017 Richard Guadagno
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

#include <thread>
#include "fr.h"
#include "FRMain.h"


FRListener::FRListener(const std::string ip, int port, const std::string& opcodeFile) : _ip(ip), _port(port), _mon(*this, opcodeFile)
{
}

int FRListener::getCount(const std::string& item)
{
   return _items[item];
}

void FRListener::signal_timed(const frenchroast::monitor::TimerReport& rpt)
{
  timersignal(rpt);
}

void FRListener::signal(const frenchroast::monitor::SignalReport& rpt)
{
  thooked(rpt);
}

void FRListener::traffic(std::vector<frenchroast::monitor::StackTrace> items)
{
  traffic_signal(items);
}

void FRListener::jammed(frenchroast::monitor::JammedReport& rpt)
{
  stack_jammed(rpt);
}

void FRListener::heap_event(frenchroast::monitor::HeapReport& rpt)
{
  heap_changed(rpt);
}

void FRListener::connected(const std::string& host, const std::string& pid)
{
  remoteconnected( host,pid);
}

void FRListener::unloaded(const std::string& host, const std::string& pid)
{
  remoteunloaded(host,pid);
}

void FRListener::ack_profiler_off(const std::string& host, const std::string& pid)
{
  remote_ack_off(host,pid);
}

void FRListener::ack_profiler_on(const std::string& host, const std::string& pid)
{
  remote_ack_on(host,pid);
}

void FRListener::init()
{
  std::thread t1{[this](){_mon.init_receiver(_ip, _port);}};
  t1.detach();
}


void FRListener::start_traffic(int rate)
{
  _mon.watch_traffic(rate);
}

void FRListener::stop_traffic()
{
  method_ranking(_mon.stop_watch_traffic());
}


void FRListener::start_watch_loading()
{
  _mon.start_watch_loading();
}

void FRListener::stop_watch_loading()
{
  _mon.stop_watch_loading();
}

void FRListener::ready(const std::string& host, const std::string& pid)
{
  remote_ready(host,pid);
}

void FRListener::request_hooks(const std::string& ipport)
{
  _signal_requests.push_back(ipport);
  send_signals();
}

void FRListener::turn_on_profiler(const std::string& hostname_pid)
{
  _mon.turn_on_profiler(hostname_pid);
}


void FRListener::turn_off_profiler(const std::string& hostname_pid)
{
  _mon.turn_off_profiler(hostname_pid);
}



void FRListener::class_watch( const std::vector<frenchroast::monitor::ClassDetail>& details)
{
  class_loaded(details);
}

void FRListener::validated_signals(std::vector<std::string> signals_from_editor)
{
  for(auto& ipport : _signal_requests) { 
    _mon.send_hooks(signals_from_editor, ipport);
  }
  _signal_requests.clear();
}

void FRListener::reset()
{
  _mon.reset();
}
