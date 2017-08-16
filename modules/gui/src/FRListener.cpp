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

#include "fr.h"
#include "FRMain.h"
#include <thread>

FRListener::FRListener(const std::string ip, int port, const std::string& opcodeFile) : _ip(ip), _port(port), _mon(*this, opcodeFile)
{
}

int FRListener::getCount(const std::string& item)
{
   return _items[item];
}

void FRListener::signal_timed(const std::string& tag, const std::string& tname, long elapsed, int last)
{
  timersignal(tag,tname,elapsed);
}

void FRListener::signal(const std::string& tag, const std::string& tname, int count, std::vector<std::string> args, std::vector<std::string> instances,  std::vector<frenchroast::monitor::MarkerField> markers,  std::unordered_map<std::string, frenchroast::monitor::StackReport> stacks)
{
  thooked(tag, tname, count, args, instances, markers, stacks);
}

void FRListener::traffic(std::vector<frenchroast::monitor::StackTrace>& items)
{
  traffic_signal(items);
}

void FRListener::jammed(frenchroast::monitor::JammedReport& rpt)
{
  stack_jammed(rpt);
}

void FRListener::connected(const std::string& host, const std::string& pid)
{
  remoteconnected( host,pid);
}

void FRListener::unloaded(const std::string& host, const std::string& pid)
{
  remoteunloaded(host,pid);
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

void FRListener::request_hooks()
{
  _hooks_request = true;
  send_hooks();
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

void FRListener::validated_hooks(std::vector<std::string> hooks)
{
  if(_hooks_request) {
    _hooks_request = false;
    _mon.send_hooks(hooks);
  }
}

