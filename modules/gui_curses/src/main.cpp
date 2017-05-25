#include <iostream>
#include <thread>
#include <ncurses.h>
#include "Monitor.h"
#include <string>
#include <signal.h>
#include "CursesFrameWork.h"
#include "SignalWindow.h"
#include "SignalTimerWindow.h"

namespace frenchroast { namespace monitor {
    //-------------------------------------------------------------------------
    void refresh_status();
    std::string _connectedMsg = "waiting for connection...";
    SignalWindow* sigptr{nullptr};
    SignalTimerWindow* sigtimerptr{nullptr};
    //-------------------------------------------------------------------------



    class EventHandler {
      SignalWindow&      _sigwin;
      SignalTimerWindow& _sigtimer_win;

    public:
      EventHandler(SignalWindow& sw, SignalTimerWindow& stw) : _sigwin(sw), _sigtimer_win(stw) {}
      void signal_timed(const std::string& tag, long long totalElapsed, int elapsed)
      {
        _sigtimer_win.add_signal(tag, totalElapsed, elapsed);
      }


      void signal(const std::string& tag, int t1)
      {
        _sigwin.add_signal(tag, t1);
      }
      
      void traffic(std::vector<StackTrace> items) {}
      void connected(const std::string& msg)
      {
        _connectedMsg = "connected: " + msg;
        refresh_status();
      }
    };

    
void refresh_status()
{
  int maxy =0;
  int maxx =0;
  refresh();
  getmaxyx(stdscr, maxy,maxx);
  --maxy;
  mvaddstr(maxy,0, _connectedMsg.c_str());    
  mvchgat(maxy,0,200, A_REVERSE, 0,NULL);
  refresh();
}

void refresh_all(int x)
{
  erase();
  endwin();
  refresh_status();
  sigptr->redraw();
  sigtimerptr->redraw();
  wrefresh(*sigptr);
  wrefresh(*sigtimerptr);
  refresh();
}


int main(int argc, char* argv[])
{
  initscr();
  cbreak();
  noecho();
  keypad(stdscr,TRUE);
  erase();

  refresh();
  SignalWindow      sig_window{8,60,1,1, "Signals"};
  SignalTimerWindow sigtimer_window{8,70,1,65, "Timers"};
  
  wrefresh(sig_window);
  refresh();

  refresh();
  wrefresh(sig_window);
  refresh();
  sigptr = &sig_window;
  sigtimerptr = &sigtimer_window;

  wrefresh(sig_window);
  wrefresh(sigtimer_window);
  refresh();

  
  int maxy = 0;
  int maxx = 0;
  getmaxyx(stdscr, maxy,maxx);
  --maxy;
  mvaddstr(maxy,0, "waiting for connection...");    
  mvchgat(maxy,0,200, A_REVERSE, 0,NULL);

  signal(SIGWINCH, refresh_all);
  
  
  
  frenchroast::monitor::EventHandler hand{sig_window, sigtimer_window};
  frenchroast::monitor::Monitor<frenchroast::monitor::EventHandler> mon{hand};

  std::thread t1{[&]() { mon.init_receiver("127.0.0.1", 6060);}};

  
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
