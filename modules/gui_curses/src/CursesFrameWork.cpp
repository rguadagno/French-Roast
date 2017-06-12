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

#include "CursesFrameWork.h"


namespace frenchroast { namespace monitor { namespace curses {

      void draw_label(WINDOW* win, int ypos, int xpos, const std::string& text)
      {
        mvwaddstr(win, ypos,xpos, text.c_str());    
        mvwchgat(win,ypos,xpos,text.length(), A_REVERSE, 0,NULL);
        wrefresh(win);
      }

      Window::Window(int rows, int cols, int ypos, int xpos, const std::string& title) :
        _rows(rows), _cols(cols), _ypos(ypos), _xpos(xpos), _title(title)
      {
        _wptr = newwin(_rows, _cols, _ypos, _xpos);
        draw_title();
        scrollok(_wptr, true);
      }

      Window::operator WINDOW*()
      {
        return _wptr;
      }

      void Window::draw_title()
      {
        size_t totalpad = (_cols - _title.length() ) / 2;
        std::string pad{"                                                                                          ",
                            totalpad};
        std::string fulltitle = pad + _title + pad ;
        draw_label(_wptr, 0,0, fulltitle);
      }
      
    }}};
