
#include "SignalWindow.h"



void frenchroast::monitor::SignalWindow::add_signal(const std::string& tag, int t1) {
  if(_sigmap[tag] == 0) {
    _sigmap[tag] = ++_row;
  }
  std::string countStr = " [" + std::to_string(t1) + "]";
  mvwaddstr(_wptr,_sigmap[tag], 0, (tag + countStr ).c_str());
  _current_map[_sigmap[tag]] = tag + countStr;
  wrefresh(_wptr);
}


void frenchroast::monitor::SignalWindow::redraw() {
  draw_title();
  refresh();
  mvwaddstr(stdscr, 10,10, std::to_string(_sigmap.size()).c_str()   );
  for(auto& item : _sigmap) {
    mvwaddstr(stdscr, item.second, 0, _current_map[item.second].c_str());
      mvwaddstr(stdscr, 10,10,"sfsdFSD");

  }
  wrefresh(_wptr);
  refresh();



}
