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

#ifndef PERSISTOR_H
#define PERSISTOR_H

#include <string>
#include <vector>
#include <unordered_map>
#include "ClassDetail.h"
#include "JammedReport.h"

namespace frenchroast { namespace session {
    class Persistor  {

      virtual const std::string& get_serial(char tag)=0;
    public:
      virtual void store(const std::string& descritpor, std::vector<std::string> items) = 0;
      virtual void load(const std::string& descriptor) = 0;
      template <typename T>
        void load(char tag, std::vector<T>& items)  {
        get_serial(tag) >> items;
      }
      virtual void store() = 0;
      virtual ~Persistor(){};
    };
  }
}

#endif
