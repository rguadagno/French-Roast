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

#ifndef FRSTACKFRAME_H
#define FRSTACKFRAME_H

#include <vector>
#include <string>
#include "Descriptor.h"

namespace frenchroast { namespace monitor {

    class StackFrame {
      friend StackFrame& operator>>(const std::string& rep, StackFrame& ref);
      Descriptor  _name;
      int         _monitorCount{0};
      
    public:
      StackFrame(Descriptor name, int count);
      StackFrame(Descriptor name);
      StackFrame() = default;
      std::string get_name()  const; 
      int         get_monitor_count()  const;
      bool operator==(const StackFrame&) const;
      bool operator!=(const StackFrame&) const;
      operator std::string() const;
      const Descriptor& descriptor() const;
    };

    template <typename OutType>
      OutType& operator<<(OutType& out, const StackFrame& ref)
      {
        out << ref.get_name() << "<end-method>" << ref.get_monitor_count();
        return out;
      }

    template <typename OutType>
      OutType& operator<<(OutType& out, const std::vector<StackFrame>& ref)
      {
        for(auto& x : ref) {
          out << x << "<end-frame>";
        }
        return out;
      }
    std::vector<StackFrame>& operator>>(const std::string& line, std::vector<StackFrame>& frames);
    
  }
}


#endif
