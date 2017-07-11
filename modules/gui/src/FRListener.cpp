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

FRListener::FRListener(const std::string ip, int port, const std::string& opcodeFile, const std::string& hooksFile ) : _ip(ip), _port(port), _mon(*this, opcodeFile, hooksFile)
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

void FRListener::signal(const std::string& tag, const std::string& tname, int count, std::vector<std::string> args, std::vector<std::string> instances,  std::vector<frenchroast::monitor::MarkerField> markers)
{
  thooked(FRMain::SignalWindow,tag, tname, count, args, instances, markers);
}

void FRListener::traffic(std::vector<frenchroast::monitor::StackTrace>& items)
{
  traffic_signal(items);
}

void FRListener::connected(const std::string& msg)
{
  remoteconnected( msg);
}

void FRListener::unloaded(const std::string& msg)
{
  remoteunloaded(msg);
}

void FRListener::init()
{
  _mon.init_receiver(_ip, _port);
}

void FRListener::start_traffic(int rate)
{
  _mon.watch_traffic(rate);
}

std::vector<frenchroast::monitor::MethodStats> FRListener::stop_traffic()
{
  return _mon.stop_watch_traffic();
}

void FRListener::ready()
{
  remote_ready();
}
