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
