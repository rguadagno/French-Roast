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
      return _frames.size();
    }

    bool StackTrace::operator==(const StackTrace& ref)
    {
      return _key == ref._key;
    }


    bool StackTrace::operator>(const StackTrace& ref)
    {
      if(_key == ref._key) return false;
      if(_frames.size() < ref._frames.size()) return false;
      auto frame = _frames.rbegin();
      for(auto rframe = ref._frames.rbegin();  rframe != ref._frames.rend(); rframe++, frame++) {
        if(*rframe != *frame) return false;
      }
      return true;
    }

    bool StackTrace::operator<(const StackTrace& ref)
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


    std::vector<std::string> StackTrace::descriptor_frames() const
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
      
      _frames.push_back(frame.get_name());
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
    
  }
}

