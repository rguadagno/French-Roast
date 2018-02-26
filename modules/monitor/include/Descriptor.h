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
#include <vector>

namespace frenchroast { namespace monitor {
    class Descriptor {
      friend     Descriptor& operator>>(const std::string&, Descriptor& ref);
      std::string _class_name;
      std::string _method_name;
      std::string _raw_param_types;
      void translate(const std::string& pname);
      std::string translate_param_types(const std::string& pstr);
      std::string translate_return_type(const std::string& name);
      std::vector<std::string> parse_type_tokens(const std::string& tstr);
    public:
      Descriptor()=default;
      explicit Descriptor(const std::string& agentRep);
      Descriptor(Descriptor&& agentRep) = default;
      Descriptor(const Descriptor& agentRep) =default;
      std::string class_name() const;
      std::string method_name() const;
      std::string full_name() const;
      std::string raw_param_types() const;
      bool        operator==(const Descriptor&) const;
      bool        operator!=(const Descriptor&) const;
      Descriptor& operator=(const Descriptor&);
      operator std::string() const;
    };

    Descriptor& operator>>(const std::string&, Descriptor& ref);
    Descriptor& operator>>(const std::string&, Descriptor&& ref);
    
    template <typename OutType>
      OutType& operator<<(OutType& out, const Descriptor& ref)
      {
        out << ref.class_name() << (ref.class_name() != "" ? "::" : "") << ref.method_name();
        return out;
      }


    std::string descriptor_to_string(const std::string& str);
    
  }
}

#endif
