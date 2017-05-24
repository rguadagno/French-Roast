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
