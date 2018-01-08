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

#include "ClassDetail.h"
#include "Util.h"
#include "MonitorUtil.h"

namespace frenchroast { namespace monitor {

    ClassDetail::ClassDetail(const std::string& name, std::vector<Descriptor>& methods) : _name(name), _methods(methods)
    {
      replace(_name,'/','.');
    }
    
    ClassDetail::ClassDetail()
    {
    }

    ClassDetail::ClassDetail(const ClassDetail& ref)
    {
      _name = ref._name;
      _methods = ref._methods;
    }
    
    const std::string& ClassDetail::name() const
    {
      return _name;
    }
      
    const std::vector<Descriptor>& ClassDetail::methods() const
    {
      return _methods;
    }

    bool ClassDetail::operator==(const ClassDetail& ref) const
    {
      if(_name != ref._name) return false;
      if(_methods.size() != ref._methods.size()) return false;
      
      for(std::size_t idx = 0;idx < _methods.size(); idx++) {
        if(_methods[idx] != ref._methods[idx]) return false;
      }
      return true;
    }

    bool ClassDetail::operator!=(const ClassDetail& ref) const
    {
      return !(*this == ref);
    }

    
    ClassDetail& operator>>(const std::string& serial, ClassDetail& ref)
    {
      ref._name = frenchroast::split(serial, "<end-name>")[0];
      for(auto& method : frenchroast::split(frenchroast::split(serial, "<end-name>")[1],"<end-method>")) {
        if(method != "") {
          Descriptor dsc{};
          method >> dsc;
          ref._methods.push_back(dsc);
        }
      }
      return ref;
    }

    std::vector<ClassDetail>& operator>>(const std::string& line, std::vector<ClassDetail>& ref)
    {
      for(auto& cd : frenchroast::split(line, ClassDetail::TAG_END)) {
        if(cd != "") {
          ClassDetail item;
          cd >> item;
          ref.push_back(item);
        }
      }
      return ref;
    }
    
    bool operator==(const std::vector<ClassDetail>& listA, const std::vector<ClassDetail>& listB)
    {
      if(listA.size() != listB.size()) return false;
      for(std::size_t idx = 0; idx < listA.size(); idx++) {
        if(listA[idx] != listB[idx]) return false;
      }
      return true;
    }

    const std::string ClassDetail::TAG_END = "<end-item>";
    
  }
}


