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
    class SignalMarkers  {
      friend     SignalMarkers& operator>>(const std::string&, SignalMarkers& ref);      
      std::vector<std::tuple<std::string,std::string>> _items;
    public:

      
      SignalMarkers(const std::vector<std::string>&);
      SignalMarkers() = default;
      const std::size_t  size() const;
      const std::string& operator[](std::size_t idx) const;      
      //      const std::vector<std::string>& items() const;
      //SignalMarkers& operator+=(const std::string&);
      //bool operator==(const SignalParams&) const;

      class Marker {
      public:
        std::string label;
        std::string value;
      };
    };

    /*
    template <typename OutType>
      OutType& operator<<(OutType& out, const SignalParams& ref)
      {
        for(auto& x : ref.items()) {
          out << x << "<end-param>";
        }
        return out;
      }

      SignalParams& operator>>(const std::string&, SignalParams& ref);      
    */


      

    
    }
  }


#endif
