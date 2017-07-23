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

namespace frenchroast { namespace monitor {
    class ClassDetail {
      std::string              _name;
      std::vector<std::string> _methods;

    public:
      ClassDetail(const std::string& name, std::vector<std::string>& descriptors);
      ClassDetail();
      const std::string& name() const;
      const std::vector<std::string>& methods() const;
    };
  }
}


#endif
