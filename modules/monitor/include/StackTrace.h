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

#ifndef FRSTACKTRACE_H
#define FRSTACKTRACE_H

#include <vector>
#include <stack>
#include <string>
#include "StackFrame.h"

namespace frenchroast { namespace monitor {

    class StackTrace {
      std::string              _key;
      std::string              _monitorkey;
      std::string              _thread_name;
      std::vector<StackFrame>  _frames;
      std::vector<int>         _monitors;

    public:
      StackTrace(const std::string tname);
      StackTrace(const StackTrace&);
      StackTrace();
      std::string               thread_name()  const;
      std::vector<StackFrame>   descriptor_frames() const;
      std::vector<int>          monitor_frames() const;
      void                      addFrame(const StackFrame& frame);
      int size() const;
      const std::string& key() const;
      bool operator==(const StackTrace&) const;
      void operator=(const StackTrace&);
      bool operator>(const StackTrace&) const;
      bool operator<(const StackTrace&) const;
      bool update_monitors(const StackTrace&);      
      
    };
    
    template <typename OutType>
      OutType& operator<<(OutType& out, const StackTrace& ref)
      {

        out << ref.thread_name() << "<end-thread-name>";
        for(auto x : ref.monitor_frames()) {
          out << x << "<end-monitor>";
        }
        out  << "<end-monitors>";
        out << ref.descriptor_frames();
        out << "<end-trace>";
        
        return out;
      }
    
    
  }
}


#endif
