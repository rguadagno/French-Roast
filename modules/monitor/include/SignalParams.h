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

#ifndef SIGNALPARAM_H
#define SIGNALPARAM_H

#include <vector>

namespace frenchroast { namespace monitor {
    class SignalParams  {
      friend     SignalParams& operator>>(const std::string&, SignalParams& ref);      
      std::vector<std::string> _items;
      std::string              _key;
    public:
      SignalParams(const std::vector<std::string>&);
      SignalParams() = default;
      const std::string&        operator[](std::size_t idx) const;
      std::size_t               size() const;
      SignalParams&             operator+=(const std::string&);
      bool                      operator==(const SignalParams&) const;
      const std::string         key() const;
      decltype(_items.cbegin()) begin() const;
      decltype(_items.cend())   end() const;
      operator std::vector<std::string>();
    };

    template <typename OutType>
      OutType& operator<<(OutType& out, const SignalParams& ref)
      {
        for(auto& x : ref) {
          out << x << "<end-param>";
        }
        return out;
      }

      SignalParams& operator>>(const std::string&, SignalParams& ref);      

    
    }
  }


#endif
