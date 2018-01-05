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

#include <vector>
#include "StackFrame.h"
#include "Util.h"

namespace frenchroast { namespace monitor {

    StackFrame::StackFrame(Descriptor name, int count) : _name(std::forward<Descriptor>(name)), _monitorCount(count)
    {
    }

    StackFrame::StackFrame(Descriptor name) : _name(std::forward<Descriptor>(name))
    {
    }

    StackFrame& operator>>(const std::string& rep, StackFrame& ref)
    {
      std::vector<std::string> items = split(rep, "<end-method>");
      ref._monitorCount = atoi(items[1].c_str());
      items[0] >> ref._name;
      return ref;
    }
    
    std::string StackFrame::get_name()  const 
    {
      return _name.full_name();
    }

    int  StackFrame::get_monitor_count()  const
    {
      return _monitorCount;
    }
    
    bool StackFrame::operator==(const StackFrame& ref) const
    {
      return _name == ref._name;
    }
    
    bool StackFrame::operator!=(const StackFrame& ref) const
    {
      return _name != ref._name;      
    }

    StackFrame::operator std::string() const
    {
      return get_name();
    }
    
  }
}
