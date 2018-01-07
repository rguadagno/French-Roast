// copyright (c) 2016 Richard Guadagno
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

#include <vector>
#include <iostream>
#include "StackTrace.h"
#include "Util.h"

namespace frenchroast { namespace monitor {

    
    StackTrace::StackTrace(const std::string tname) : _thread_name(tname), _key("")
    {
    }

    StackTrace::StackTrace() : StackTrace("")
    {
    }
    
    StackTrace::StackTrace(const StackTrace& ref)
    {
      _key = ref._key;
      _thread_name = ref._thread_name;
      _frames = ref._frames;
      _monitors = ref._monitors;
    }

    void StackTrace::operator=(const StackTrace& ref)
    {
      _key = ref._key;
      _thread_name = ref._thread_name;
      _frames = ref._frames;
      _monitors = ref._monitors;
    }
    
    int StackTrace::size() const
    {
      return static_cast<int>(_frames.size());
    }

    bool StackTrace::operator==(const StackTrace& ref) const
    {
      return _key == ref._key;
    }


    bool StackTrace::operator>(const StackTrace& ref) const
    {
      if(_key == ref._key) return false;
      if(_frames.size() < ref._frames.size()) return false;
      auto frame = _frames.rbegin();
      for(auto rframe = ref._frames.rbegin();  rframe != ref._frames.rend(); rframe++, frame++) {
        if(*rframe != *frame) return false;
      }
      return true;
    }

    bool StackTrace::operator<(const StackTrace& ref) const
    {
      return const_cast<StackTrace&>(ref) > *this;
    }

    bool StackTrace::update_monitors(const StackTrace& ref)
    {
      bool rv = false;
      StackTrace* larger = nullptr;
      StackTrace* smaller = nullptr;
      if(_monitors.size() < ref._monitors.size()) {
        larger = const_cast<StackTrace*>(&ref);
        smaller = this;
      }
      else {
        larger =  this;
        smaller = const_cast<StackTrace*>(&ref);
      }

      auto rmonitor = larger->_monitors.rbegin();
      for(auto monitor = smaller->_monitors.rbegin();  monitor != smaller->_monitors.rend(); monitor++, rmonitor++) {
        if(*rmonitor > 0 && *monitor == 0 ) {
          *monitor = 1;
          rv = true;
        }
        if(*monitor > 0 && *rmonitor == 0 ) {
          *rmonitor = 1;
          rv = true;
        }
        
      }
      return rv;
    }

    void StackTrace::clear()
    {
      _key = "";
      _monitorkey = "";
      _thread_name = "";
      _frames.clear();
      _monitors.clear();
    }
    
    std::vector<StackFrame> StackTrace::descriptor_frames() const
    {
      return _frames;
    }

    std::vector<int> StackTrace::monitor_frames() const
    {
      return _monitors;
    }

    void StackTrace::addFrame(const StackFrame& frame) 
    {
      _key += frame.get_name();
      _monitorkey += frame.get_monitor_count() > 0 ? "X" : "0";
      
      _frames.push_back(frame);
      _monitors.push_back(frame.get_monitor_count());
    }

    std::string StackTrace::thread_name()  const 
    {
      return _thread_name;
    }
    
    const std::string& StackTrace::key()  const 
    {
      return _key;
    }


    StackTrace& operator>>(const std::string& rep, StackTrace& ref)
    {
      ref.clear();
      std::vector<std::string> parts = frenchroast::split(rep,"<end-thread-name>");
      ref._thread_name = parts[0];
      parts = frenchroast::split(parts[1], "<end-monitors>");
      for(auto& mon : frenchroast::split(parts[0], "<end-monitor>")) {
        ref._monitors.push_back(atoi(mon.c_str()));
      }
      for(auto& framestr : frenchroast::split(parts[1],"<end-frame>")) {
        if(framestr == "") continue;
        StackFrame frame{};
        ref.addFrame(framestr >> frame);
      }
      
      return ref;
    }
    
  }
}

