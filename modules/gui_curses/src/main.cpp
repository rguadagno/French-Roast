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
#include "Monitor.h"
#include <string>

namespace frenchroast { namespace monitor {
    int main(int argc, char* argv[]);
  }}

int main(int argc, char* argv[])
{
  frenchroast::monitor::main(argc, argv);
}


namespace frenchroast { namespace monitor {

    void get_cmd_line_args(int argc, char* argv[], std::string& url, int& port) noexcept;
    
    class EventHandler {

    public:
      frenchroast::monitor::Monitor<frenchroast::monitor::EventHandler> _monitor;
      EventHandler();
      void signal_timed(const std::string& tag, const std::string& tname, long long totalElapsed, int elapsed);
      void signal(const std::string& tag, const std::string& tname, int count,  std::vector<std::string>,std::vector<std::string>, std::vector<frenchroast::monitor::MarkerField>, std::unordered_map<std::string, frenchroast::monitor::StackReport>);
      void traffic(std::vector<StackTrace> items);
      void connected(const std::string& msg,const std::string& );
      void unloaded(const std::string&, const std::string& msg);
      void jammed(frenchroast::monitor::JammedReport&);
      void class_watch(const std::vector<frenchroast::monitor::ClassDetail>& details);
      void ready(const std::string& host, const std::string& pid);
      void request_hooks();
    };
    
    std::string _connectedMsg = "waiting for connection...";
      
    int main(int argc, char* argv[])
    {
      std::string url;
      int port;
      get_cmd_line_args(argc, argv, url, port);
  
      frenchroast::monitor::EventHandler hand{};
      
      std::thread t1{[&]() { hand._monitor.init_receiver(url, port);}};
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
      
      ::signal(SIGWINCH, refresh_all);
  
      frenchroast::monitor::EventHandler hand{sig_window, sigtimer_window};

      // XXX Disabled because doesn't compile.
      // frenchroast::monitor::Monitor<frenchroast::monitor::EventHandler> mon{hand};
      // std::thread t1{[&]() { mon.init_receiver(url, port);}};
  
      bool running = true;

      while(running) {
        std::string line;
        std::cin >> line;
        std::cout << "line: " << line << std::endl;
        }
        std::cout << "EXITED:" << std::endl;
      }
      
  

    
    EventHandler::EventHandler() : _monitor(*this, "/home/richg/code/French-Roast/modules/instrumentation/config/opcodes.txt")
    {
    }

    void EventHandler::signal_timed(const std::string& tag, const std::string& tname, long long totalElapsed, int elapsed)
    {
    }

    void EventHandler::signal(const std::string& tag, const std::string& tname, int count,  std::vector<std::string>,std::vector<std::string>, std::vector<frenchroast::monitor::MarkerField>, std::unordered_map<std::string, frenchroast::monitor::StackReport>)
    {
      std::cout << tag << std::endl;
    }
      
    void EventHandler::traffic(std::vector<StackTrace> items)
    {
    }
      
    void EventHandler::connected(const std::string& msg,const std::string& np)
    {
      std::cout << "connected" << std::endl;
      // _monitor.turn_on_profiler(msg+np);
      _connectedMsg = "connected: " + msg;
      }

    void EventHandler::unloaded(const std::string& ipport, const std::string& msg)
    {
      _connectedMsg = "disconnected: " + msg;
    }

    void EventHandler::jammed(frenchroast::monitor::JammedReport&)
    {
    }

    void EventHandler::class_watch(const std::vector<frenchroast::monitor::ClassDetail>& details)
    {
    }
    
    void EventHandler::ready(const std::string& host, const std::string& pid)
    {
           std::cout << "READY." << std::endl;
           _monitor.turn_on_profiler(host+pid);
    }
    
    void EventHandler::request_hooks()
    {
      std::vector<std::string> hooks;
      hooks.push_back("juniper.Berry::bigcalc:(int):void <ENTER>");

      _monitor.send_hooks(hooks);
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

