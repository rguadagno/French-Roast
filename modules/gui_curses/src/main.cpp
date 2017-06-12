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
    int main(int argc, char* argv[]);
  }}

int main(int argc, char* argv[])
{
  frenchroast::monitor::main(argc, argv);
}


namespace frenchroast { namespace monitor {

    void refresh_status();
    void get_cmd_line_args(int argc, char* argv[], std::string& url, int& port) noexcept;
    void refresh_all(int x);
    
    class EventHandler {
      SignalWindow&      _sigwin;
      SignalTimerWindow& _sigtimer_win;

    public:
      EventHandler(SignalWindow& sw, SignalTimerWindow& stw);
      void signal_timed(const std::string& tag, long long totalElapsed, int elapsed);
      void signal(const std::string& tag, int t1);
      void traffic(std::vector<StackTrace> items);
      void connected(const std::string& msg);
    };
    
    std::string _connectedMsg = "waiting for connection...";
    SignalWindow* sigptr{nullptr};
    SignalTimerWindow* sigtimerptr{nullptr};
    
    int main(int argc, char* argv[])
    {
      std::string url;
      int port;
      get_cmd_line_args(argc, argv, url, port);
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
      
      std::thread t1{[&]() { mon.init_receiver(url, port);}};
  
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

    
    EventHandler::EventHandler(SignalWindow& sw, SignalTimerWindow& stw) : _sigwin(sw), _sigtimer_win(stw)
    {
    }

    void EventHandler::signal_timed(const std::string& tag, long long totalElapsed, int elapsed)
    {
      _sigtimer_win.add_signal(tag, totalElapsed, elapsed);
    }

    void EventHandler::signal(const std::string& tag, int t1)
    {
      _sigwin.add_signal(tag, t1);
    }
      
    void EventHandler::traffic(std::vector<StackTrace> items)
    {
    }
      
    void EventHandler::connected(const std::string& msg)
    {
      _connectedMsg = "connected: " + msg;
      refresh_status();
    }
   

    
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

    void get_cmd_line_args(int argc, char* argv[], std::string& url, int& port) noexcept
    {
      if(argc != 2) {
        throw std::invalid_argument("usage: roaster <url:port>");
      }
      std::string tstr{argv[1]};
      std::vector<std::string> parts = frenchroast::split(tstr, ":");
      if(parts.size() != 2) {
         throw std::invalid_argument("\nincorrect url:port: " + tstr + "\nusage: roaster <url:port>");
      }
      port = std::stoi(parts[1]);
      url = parts[0];
    }
    
    
  }}

