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
#include "Util.h"
#include "Descriptor.h"
#include "MonitorUtil.h"

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
