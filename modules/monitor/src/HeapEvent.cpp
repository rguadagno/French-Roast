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


#include "HeapEvent.h"
#include "Util.h"

namespace frenchroast { namespace monitor {
    HeapEvent::HeapEvent(const char* classnm, long long tag) : _class_name(classnm), _tag(tag)
    {
      _class_name = _class_name.substr(1);
      replace(_class_name,"/",".");

    }
    
    HeapEvent::HeapEvent(long long tag) : _tag(tag)
    {
      _free_event = true;
    }
    
    const std::string& HeapEvent::classname() const
    {
      return _class_name;
    }
    
    long long HeapEvent::tag() const
    {
      return _tag;
    }
    
    bool HeapEvent::is_free() const
    {
      return _free_event;
    }

    HeapEvent& operator>>(const std::string& rep, HeapEvent&& ref)
    {
      return rep >> ref;
    }
    
    HeapEvent& operator>>(const std::string& rep, HeapEvent& ref)
    {
      auto parts = frenchroast::split(rep,"<end-type>");
      ref._tag = atoll(frenchroast::split(parts[1],"<end-tag>")[0].c_str());
      if(parts[0] == "free") {
        ref._free_event = true;

      }
      else {
        ref._class_name = frenchroast::split(frenchroast::split(parts[1],"<end-classname>")[0],"<end-tag>")[1];
      }
      return ref;
    }
      

  }
}
