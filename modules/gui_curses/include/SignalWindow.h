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
