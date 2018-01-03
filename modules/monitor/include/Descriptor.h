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

#ifndef DESCRIPTOR_H
#define DESCRIPTOR_H
#include <string>
#include <iostream>

namespace frenchroast { namespace monitor {
    class Descriptor {
      friend     Descriptor& operator>>(const std::string&, Descriptor& ref);
      std::string _class_name;
      std::string _method_name;
      void translate(const std::string& pname);

    public:
      Descriptor()=default;
      Descriptor(const std::string& agentRep);
      Descriptor(Descriptor&& agentRep) = default;
      Descriptor(const Descriptor& agentRep) =default;
      std::string class_name() const;
      std::string method_name() const;
      std::string full_name() const;
      bool operator==(const Descriptor&) const;
      bool operator!=(const Descriptor&) const;
      Descriptor& operator=(const Descriptor&);

    };

    Descriptor& operator>>(const std::string&, Descriptor& ref);
    template <typename OutType>
      OutType& operator<<(OutType& out, const Descriptor& ref)
      {
        out << ref.class_name() << (ref.class_name() != "" ? "::" : "") << ref.method_name();
        return out;
      }

    
  }
}

#endif
