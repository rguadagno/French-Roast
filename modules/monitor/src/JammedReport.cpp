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
#include "JammedReport.h"

namespace frenchroast { namespace monitor {


    JammedReport::JammedReport(StackTrace waiter, StackTrace owner) : _waiter(waiter), _owner(owner)
    {
      _key = _waiter.key() + _owner.key();
    }
    
    JammedReport::JammedReport()
    {
    }

    std::vector<std::string> JammedReport::monitors() const
    {
      std::vector<std::string> rv;
      for(auto& x : _monitors) {
        rv.push_back(x);
      }
      return rv;
    }
    
    JammedReport& JammedReport::add_monitor(const std::string& mon)
    {
      _monitors.insert(mon);
      ++_count;
      return *this;
    }

    const std::string& JammedReport::key()  const 
    {
      return _key;
    }

    const StackTrace& JammedReport::waiter() const
    {
      return _waiter;
    }
    
    const StackTrace& JammedReport::owner() const
    {
      return _owner;
    }

    
  }
}

