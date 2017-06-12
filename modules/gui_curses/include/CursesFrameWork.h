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

#ifndef CUR_FRAMEWORK_H
#define CUR_FRAMEWORK_H

#include <ncurses.h>
#include <string>

namespace frenchroast { namespace monitor { namespace curses {

      void draw_label(WINDOW* win, int ypos, int xpos, const std::string& text);

      
      class Window {
      public:
        Window(int rows, int cols, int ypos, int xpos, const std::string& title);
        virtual void redraw() = 0;
        void draw_title();
        operator WINDOW*();

      protected:
        WINDOW* _wptr{nullptr};
      private:        
        int _rows;
        int _cols;
        int _ypos;
        int _xpos;
        const std::string _title;
      };


    }}};


#endif
