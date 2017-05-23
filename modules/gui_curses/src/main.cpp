#include <iostream>
#include <thread>
#include <ncurses.h>
#include "Monitor.h"
#include <string>
#include <signal.h>

int maxy=0;
int maxx=0;
std::string _connectedMsg = "waiting for connection...";
void refresh_status();


void draw_label(WINDOW* win, int ypos, int xpos, const std::string& text) {
  mvwaddstr(win, ypos,xpos, text.c_str());    
  mvwchgat(win,ypos,xpos,text.length(), A_REVERSE, 0,NULL);
  //refresh();
  wrefresh(win);
}

namespace frenchroast { namespace monitor {

class BasicHandler {
  WINDOW* _wptr;
  int row =1;
  std::unordered_map<std::string, int> _sigmap;
public:
  BasicHandler(WINDOW* ptr) : _wptr(ptr) {}
  void signal_timed(const std::string& str, int t1, int t2) {}
  void signal(const std::string& tag, int t1) {
    if(_sigmap[tag] == 0) {
      _sigmap[tag] = ++row;;
    }
    mvwaddstr(_wptr,_sigmap[tag], 0, (tag + " [" + std::to_string(t1) + "]").c_str());
    wrefresh(_wptr);
  }
  //    mvwchgat(0,0,200, A_REVERSE, 0,NULL);

  
void traffic(std::vector<StackTrace> items) {}
void connected(const std::string& msg) {
  _connectedMsg = "connected: " + msg;
  refresh_status();
}
};

}}

WINDOW* win;




void refresh_status() {
    int maxy =0;
    int maxx =0;
    refresh();
    getmaxyx(stdscr, maxy,maxx);
    --maxy;
    mvaddstr(maxy,0, _connectedMsg.c_str());    
    mvchgat(maxy,0,200, A_REVERSE, 0,NULL);
    refresh();

}

void doit(int x) {
  
    erase();
    endwin();
    refresh_status();
    refresh();
}




int main(int argc, char* argv[]) {
  initscr();
  cbreak();
  noecho();
  WINDOW* win = stdscr;

  signal(SIGWINCH,doit);
  
  keypad(win,TRUE);
  erase();
  
  WINDOW* sigwin = newwin(8, 45, 0,0);

  scrollok(sigwin,true);
  
   refresh();
  wrefresh(sigwin);

  
  draw_label(sigwin,0,0,"         Signals         ");

  getmaxyx(win, maxy,maxx);
  --maxy;
  mvaddstr(maxy,0, "waiting for connection...");    
  mvchgat(maxy,0,200, A_REVERSE, 0,NULL);


  
  frenchroast::monitor::BasicHandler hand{sigwin};
  frenchroast::monitor::Monitor<frenchroast::monitor::BasicHandler> mon{hand};

  std::thread t1{[&]() { mon.init_receiver("127.0.0.1", 6060);}};





   
  //   t1.detach();
  const int QUIT      = 113;
  const int ENTER     = 10;
  bool running = true;

   while(running) {
    int ch = getch();
    switch(ch) {
    case ENTER:

      //      redrawwin(curscr);

      //     
      doupdate();
      //redrawwin(win);
          refresh();
      //wrefresh(sigwin);

      // wrefresh(win);

      //      redrawwin(sigwin);
      //wrefresh(sigwin);

      break;
    case QUIT:
      endwin();
      delwin(sigwin);
      exit(0);
      break;
    }
     }
   
}
