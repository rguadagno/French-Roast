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

#ifndef METHSTATS_H
#define METHSTATS_H

#include <string>

namespace frenchroast { namespace monitor {
    class MethodStats {

      std::string _descriptor;
      int         _total_invokes{0};
      
    public:
      MethodStats();
      MethodStats(std::string desc);
      MethodStats& operator++();
      const std::string& descriptor() const;
      int invoked_count() const;
    };
    int operator<(const MethodStats& m1, const MethodStats& m2);
  }
}


#endif
