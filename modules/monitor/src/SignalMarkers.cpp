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
    }
    

    //const std::vector<std::string>& SignalParams::items() const
    // {
    // return _items;
    //}
    
    const std::size_t  SignalMarkers::size() const
    {
      return _items.size();
    }

    /*
    SignalParams& SignalParams::operator+=(const std::string& param)
    {
      _items.push_back(param);
      return *this;
    }

    bool SignalParams::operator==(const SignalParams& p) const
    {
      return _items == p._items;
    }
    
    SignalParams& operator>>(const std::string& rep, SignalParams& ref)
    {
      for(auto& param : frenchroast::split(rep, "<end-param>")) {
        if(param != "") {
          ref += param;
        }
      }
      return ref;
    }
    */    
  }
}
