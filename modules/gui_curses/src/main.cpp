#include <iostream>
#include <thread>
#include <ncurses.h>
#include "Monitor.h"
#include <string>
#include <signal.h>
#include "CursesFrameWork.h"
#include "SignalWindow.h"

namespace frenchroast { namespace monitor {
    //-------------------------------------------------------------------------
    void refresh_status();
    std::string _connectedMsg = "waiting for connection...";
    SignalWindow* sigptr{nullptr};


    //-------------------------------------------------------------------------



    class EventHandler {
      SignalWindow& _sigwin;

    public:
      EventHandler(SignalWindow& sw) : _sigwin(sw) {}
      void signal_timed(const std::string& str, int t1, int t2) {}
      void signal(const std::string& tag, int t1) {
        _sigwin.add_signal(tag, t1);
      }
      
      void traffic(std::vector<StackTrace> items) {}
      void connected(const std::string& msg) {
        _connectedMsg = "connected: " + msg;
        refresh_status();
      }
    };

    
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

void refresh_all(int x) {
  
    erase();
    endwin();
    refresh_status();
    sigptr->redraw();
    wrefresh(*sigptr);
    refresh();
}


int main(int argc, char* argv[]) {
  initscr();
  cbreak();
  noecho();
  keypad(stdscr,TRUE);
  erase();

  refresh();
  SignalWindow sig_window{8,45,0,0, "Signals"};
  sigptr = &sig_window;

  wrefresh(sig_window);
  refresh();

  
  int maxy = 0;
  int maxx = 0;
  getmaxyx(stdscr, maxy,maxx);
  --maxy;
  mvaddstr(maxy,0, "waiting for connection...");    
  mvchgat(maxy,0,200, A_REVERSE, 0,NULL);

  signal(SIGWINCH, refresh_all);
  
  
  
  frenchroast::monitor::EventHandler hand{sig_window};
  frenchroast::monitor::Monitor<frenchroast::monitor::EventHandler> mon{hand};

  std::thread t1{[&]() { mon.init_receiver("127.0.0.1", 6060);}};

  



   
  //   t1.detach();
  const int QUIT      = 113;
  const int ENTER     = 10;
  bool running = true;

   while(running) {
    int ch = getch();
    switch(ch) {
    case QUIT:

        endwin();
      exit(0);
            
      break;
    }
     }
   
}
}}

int main(int argc, char* argv[]) {
  frenchroast::monitor::main(argc, argv);

}
