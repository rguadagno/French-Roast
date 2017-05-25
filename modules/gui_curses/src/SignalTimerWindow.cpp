
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



