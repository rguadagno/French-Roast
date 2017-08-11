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
#include "StackTrace.h"
#include <iostream>

namespace frenchroast { namespace monitor {

    StackTrace::StackTrace(const std::string tname) : _thread_name(tname), _key("")
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
    
    int StackTrace::size()
    {
      return _frames.size();
    }

    
    std::string StackTrace::key()  const
    {
      return _key;
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
      if(_key == ref._key) return false;
      if(_frames.size() > ref._frames.size()) return false;
      auto frame = ref._frames.rbegin();
      for(auto rframe = _frames.rbegin();  rframe != _frames.rend(); rframe++, frame++) {
        if(*rframe != *frame) return false;
      }
      return true;

    }

    
    std::string StackTrace::monitor_key()  const
    {
      return _monitorkey;
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
  }
}

