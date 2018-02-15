// copyright (c) 2018 Richard Guadagno
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

#include <string>
#include "HeapReport.h"

namespace frenchroast { namespace monitor {
    
    HeapReport& HeapReport::operator+=(const HeapEvent& hevent)
      {
        ++_lifetime;
        ++_current;
        if(_current > _max) {
          _max = _current;
        }
        if(_stacks.find(hevent.report().key()) == _stacks.end()) {
          _stacks[hevent.report().key()] = hevent.report();
        }
        else {
          ++_stacks[hevent.report().key()];
        }
        return *this;
      }
    
      HeapReport& HeapReport::operator-=(const HeapEvent& hevent)
      {
        --_current;
        return *this;
      }

      const std::unordered_map<std::string, StackReport>& HeapReport::stacks() const
      {
        return _stacks;
      }

      bool HeapReport::first()
      {
        if(_first) {
          _first = false;
          return true;
        }
        return false;
      }
    
      void HeapReport::set_classname(const std::string& str)
      {
        _class_name = str;
      }

    const std::string& HeapReport::classname() const
    {
      return _class_name;
    }
    
    int HeapReport::current() const
    {
      return _current;
    }
    
    int HeapReport::max() const
    {
      return _max;
    }
    
    int HeapReport::lifetime() const
    {
      return _lifetime;
    }
    
  }
}
