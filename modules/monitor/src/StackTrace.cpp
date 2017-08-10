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

namespace frenchroast { namespace monitor {

    StackTrace::StackTrace(const std::string tname) : _thread_name(tname), _key("")
    {
    }

    std::string StackTrace::key()  const
    {
      return _key;
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
      _frames.push_back(frame.get_name());
      _monitors.push_back(frame.get_monitor_count());
    }

    std::string StackTrace::thread_name()  const 
    {
      return _thread_name;
    }
    std::stack<std::string> StackTrace::keys() const
    {
      std::stack<std::string> rv;
      for(auto& x: _frames) {
        rv.push(x);
      }
      return rv;
    }
  }
}

