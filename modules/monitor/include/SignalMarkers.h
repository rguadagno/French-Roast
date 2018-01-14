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

#ifndef SIGNALMARKER_H
#define SIGNALMARKER_H

#include <vector>

namespace frenchroast { namespace monitor {
    class Marker {
      public:
    Marker(const std::string& l, const std::string& v) : label(l), value(v){}
        std::string label;
        std::string value;
        bool operator==(const Marker& ref) const {  return label == ref.label && value == ref.value; }
      };

    class SignalMarkers  {
      friend     SignalMarkers& operator>>(const std::string&, SignalMarkers& ref);

      std::vector<Marker> _items;

    public:
      SignalMarkers(const std::vector<std::string>&);
      SignalMarkers() = default;
      const std::size_t         size() const;
      Marker                    operator[](std::size_t idx) const;      
      SignalMarkers&            operator+=(const Marker&);
      bool                      operator==(const SignalMarkers&) const;
      decltype(_items.cbegin()) begin() const;
      decltype(_items.cend())   end() const;

    };

    
    template <typename OutType>
      OutType& operator<<(OutType& out, const SignalMarkers& ref)
      {
        for(auto& item : ref) {
          out << item.label << "<end-label>" << item.value << "<end-mark>";
        }
        return out;
      }

      SignalMarkers& operator>>(const std::string&, SignalMarkers& ref);      
    }
  }


#endif
