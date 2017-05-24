#ifndef SIG_WIN_H
#define SIG_WIN_H

#include <string>
#include <unordered_map>
#include "CursesFrameWork.h"

namespace frenchroast { namespace monitor {

    class SignalWindow : public curses::Window {

    public:
      using curses::Window::Window;     
      void add_signal(const std::string& tag, int t1);
      void redraw();
      
    private:
      int _row = 0;
      std::unordered_map<std::string, int> _sigmap;
      std::unordered_map<int, std::string> _current_map;
    };

  }};

#endif
