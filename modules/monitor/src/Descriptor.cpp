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

#include <sstream>
#include <unordered_map>
#include "Util.h"
#include "Descriptor.h"


namespace frenchroast { namespace monitor {

    Descriptor::Descriptor(const std::string& rep)
    {
      translate(rep);
    }

    void Descriptor::translate(const std::string& rep)
    {
      std::string name = rep;
      if(rep.find("::") != std::string::npos) {    
        _class_name = split(rep,"::")[0].substr(1);
        replace(_class_name,'/','.');
        replace(_class_name,';');
        name = split(rep,"::")[1];
      }
      
      std::string methodname = split(name, ":")[0];
      std::string rvstr = translate_return_type(split(split(name,")")[1],":")[0]);
      _raw_param_types = split(split(name,"(")[1],")")[0];
      std::string parms = translate_param_types(_raw_param_types);
      _raw_param_types = "(" + _raw_param_types + ")";
      _method_name = methodname + ":(" + parms + "):" + rvstr;
    }


    std::unordered_map<char, std::string> _type_map { {'I',"int"},
                                                      {'Z',"bool"},
                                                      {'V',"void"},
                                                      {'J',"long"},
                                                      {'B',"byte"},
                                                      {'C',"char"},
                                                      {'D',"double"},
                                                      {'F',"float"},
                                                      {'S',"short"}
                      
    };

    
    std::vector<std::string> Descriptor::parse_type_tokens(const std::string& tstr)
    {
      std::vector<std::string> rv;
      std::size_t pos = 0;

      while(pos < tstr.length() ) {
        std::string suffix = "";
        if(tstr[pos] == '[') {
          suffix = "[]";
          ++pos;
        }
        if (tstr[pos] == 'L') {
          int nextsemi = static_cast<int>(tstr.find(";",pos));
          rv.push_back(tstr.substr(pos+1,nextsemi-(pos+1)) + suffix );
          pos = nextsemi +1;
        }
        else {
          rv.push_back(_type_map[tstr[pos]] + suffix);
          ++pos;
        }
      }
      return rv;
    }


    
    std::string Descriptor::translate_param_types(const std::string& pstr)
    {
      std::string rv = "";

      for(auto& token : parse_type_tokens(pstr)) {
        rv.append(token);
        rv.append(",");
      }
      if(rv.length() > 1) {
        rv.erase(rv.length()-1);
      }
      return rv;
    }

    std::string Descriptor::translate_return_type(const std::string& name)
    {
      return parse_type_tokens(name)[0];
    }


    std::string Descriptor::raw_param_types() const
    {
      return _raw_param_types;
    }
    
    std::string Descriptor::class_name() const
    {
      return _class_name;
    }

    std::string Descriptor::method_name() const
    {
      return _method_name;
    }

    std::string Descriptor::full_name() const
    {
      return _class_name + "::" + _method_name;
    }

    bool Descriptor::operator==(const Descriptor& ref) const
    {
      return _class_name == ref._class_name && _method_name == ref._method_name;
    }
    
    bool Descriptor::operator!=(const Descriptor& ref) const
    {
      return !(*this == ref);
    }
    
    Descriptor& Descriptor::operator=(const Descriptor& ref)
    {
      _class_name = ref._class_name;
      _method_name = ref._method_name;
      return *this;
    }

    Descriptor::operator std::string() const
    {
      return full_name();
    }

    Descriptor& operator>>(const std::string& rep, Descriptor&& ref)
    {
      return rep >> ref;
    }
    
    Descriptor& operator>>(const std::string& rep, Descriptor& ref)
    {
      ref._class_name = "";
      ref._method_name = rep;
      if(rep.find("::") != std::string::npos) {    
        ref._class_name = split(rep,"::")[0];
        ref._method_name = split(rep,"::")[1];
      }
      return ref;
    }    

    std::string descriptor_to_string(const std::string& str)
    {
      Descriptor dsc{str};
      std::stringstream ss;
      ss << dsc;
      return ss.str();
    }
    
  }
}
