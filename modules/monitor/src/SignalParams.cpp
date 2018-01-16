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
#include "Signal.h"
#include "Util.h"

namespace frenchroast { namespace monitor {

    SignalParams::SignalParams(const std::vector<std::string>& items) : _items(items)
    {
    }
    
    const std::string& SignalParams::operator[](std::size_t idx) const
    {
      return _items[idx];
    }
    
    const std::size_t  SignalParams::size() const
    {
      return _items.size();
    }

    SignalParams& SignalParams::operator+=(const std::string& param)
    {
      _items.push_back(param);
      _key += param;
      return *this;
    }

    bool SignalParams::operator==(const SignalParams& p) const
    {
      return _key == p._key;
    }

    decltype(SignalParams::_items.cbegin()) SignalParams::begin() const
    {
      return _items.cbegin();
       
    }
    
    decltype(SignalParams::_items.cend()) SignalParams::end() const
    {
      return _items.cend();
    }

    SignalParams::operator std::vector<std::string>()
    {
      return _items;
    }

    const std::string SignalParams::key() const
    {
      return _key;
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
    
  }
}
