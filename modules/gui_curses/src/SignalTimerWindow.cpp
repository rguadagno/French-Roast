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

#include "SignalTimerWindow.h"
#include "MonitorUtil.h"


void frenchroast::monitor::SignalTimerWindow::add_signal(const std::string& tag, long  totalElapsed, int elapsed)
{
  
  if(_sigmap[tag] == 0) {
    _sigmap[tag] = ++_row;
  }
  std::string countStr = " [" + format_millis(totalElapsed) + "]";
  mvwaddstr(_wptr,_sigmap[tag], 0, (tag + countStr ).c_str());
  _current_map[_sigmap[tag]] = tag + countStr;
  wrefresh(_wptr);
  
}


void frenchroast::monitor::SignalTimerWindow::redraw()
{
  draw_title();
  refresh();
  for(auto& item : _sigmap) {
    mvwaddstr(stdscr, item.second, 0, _current_map[item.second].c_str());
  }
  wrefresh(_wptr);
  refresh();
}



