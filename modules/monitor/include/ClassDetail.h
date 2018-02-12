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

#ifndef CLASSDETAIL_H
#define CLASSDETAIL_H
#include <string>
#include <vector>
#include "Descriptor.h"

namespace frenchroast { namespace monitor {
    class ClassDetail {
      friend     ClassDetail& operator>>(const std::string&, ClassDetail& ref);
      std::string              _name;
      std::vector<Descriptor> _methods;

    public:
      ClassDetail(const std::string& name, std::vector<Descriptor>& descriptors);
      ClassDetail();
      ClassDetail(const ClassDetail&);
      const std::string& name() const;
      const std::vector<Descriptor>& methods() const;
      bool operator==(const ClassDetail&) const;
      bool operator!=(const ClassDetail&) const;

      const static std::string TAG;
      const static std::string TAG_END;
    };

    ClassDetail& operator>>(const std::string&, ClassDetail& ref);
    std::vector<ClassDetail>& operator>>(const std::string&, std::vector<ClassDetail>& ref);
    bool operator==(const std::vector<ClassDetail>& listA, const std::vector<ClassDetail>& listB);
    
    template <typename OutType>
      OutType& operator<<(OutType& out, const ClassDetail& ref)
      {
        out << ref.name() << "<end-name>";
        for(auto& x : ref.methods()) {
          out << x << "<end-method>";
        }
        return out;
      }

    template <typename OutType>
      OutType& operator<<(OutType& out, const std::vector<ClassDetail>& ref)
      {
        if(ref.size() == 0) return out;
        for(auto& x : ref) {
          out << x << ClassDetail::TAG_END.c_str();
        }
        return out;
      }

  }
}


#endif
