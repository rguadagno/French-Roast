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


#include <iostream>
#include "SignalMarkers.h"
#include "Util.h"

namespace frenchroast { namespace monitor {

    SignalMarkers::SignalMarkers(const std::vector<std::string>& items) 
    {
      for(auto& x : items) {
        _items.emplace_back(frenchroast::split(x,":")[0], frenchroast::split(x,":")[1]);
      }
      for(auto& x : _items) {
        _key += x.label + x.value;
      }
    }
    
    Marker SignalMarkers::operator[](std::size_t idx) const
    {
     return Marker{_items[idx]};
    }
    
    std::size_t  SignalMarkers::size() const
    {
      return _items.size();
    }

    SignalMarkers& SignalMarkers::operator+=(const Marker& mark) 
    {
      _items.push_back(mark);
      _key += mark.label + mark.value;
      return *this;
    }
    

    bool SignalMarkers::operator==(const SignalMarkers& ref) const
    {
      return _key == ref._key;
    }

    const std::string SignalMarkers::key() const
    {
      return _key;
    }
    
    decltype(SignalMarkers::_items.cbegin()) SignalMarkers::begin() const
    {
      return _items.cbegin();
       
    }
    
    decltype(SignalMarkers::_items.cend()) SignalMarkers::end() const
    {
      return _items.cend();
    }

    
    SignalMarkers& operator>>(const std::string& rep, SignalMarkers& ref)
    {
      ref._items.clear();
      ref._key = "";
      for(auto& mark : frenchroast::split(rep, "<end-mark>")) {
        if(mark == "") continue;
        ref += {frenchroast::split(mark,"<end-label>")[0], frenchroast::split(mark,"<end-label>")[1]};
      }
      return ref;
    }
    
  }
}
