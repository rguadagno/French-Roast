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
#include "Util.h"

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
    
    JammedReport& JammedReport::add_monitor(const std::string& mon, bool xform)
    {
      std::string str = mon;
      if(xform) {
        str = str .substr(1);
        replace(str,"/",".");
      }
      _monitors.insert(str);
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
    bool JammedReport::operator==(const JammedReport& ref) const
    {
      if(_monitors != ref._monitors) return false;
      if(_key != ref._key) return false;
      return true;;
    }
    
    JammedReport& operator>>(const std::string& rep, JammedReport& ref)
    {
      auto parts = frenchroast::split(rep,"<end-jmonitors>");
      for(auto& mon : frenchroast::split(parts[0],"<end-monitor>")) {
        if(mon == "") continue;
        ref.add_monitor(mon,false);
      }
            
      frenchroast::split(parts[1],"<end-owner>")[0] >> ref._owner;
      frenchroast::split(parts[1],"<end-owner>")[1] >> ref._waiter;
      ref._key = ref._waiter.key() + ref._owner.key();
      return ref;
    }

    JammedReport& JammedReport::operator+=(const JammedReport& ref)
    {
      for(auto& mon : ref.monitors()) {
        add_monitor(mon,false);
      }
    }


    std::vector<JammedReport>& operator>>(const std::string& line, std::vector<JammedReport>& ref)
    {
      for(auto& cd : frenchroast::split(line, JammedReport::TAG_END)) {
        if(cd != "") {
          JammedReport item;
          cd >> item;
          ref.push_back(item);
        }
      }
      return ref;
    }

    
    const  std::string JammedReport::TAG     = "<jammed>";
    const  std::string JammedReport::TAG_END = "<end-jammed>";
  }
}

