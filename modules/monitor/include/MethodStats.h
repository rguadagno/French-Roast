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
#include <vector>

namespace frenchroast { namespace monitor {
    class MethodStats {
      friend     MethodStats& operator>>(const std::string&, MethodStats& ref);
      std::string _descriptor;
      int         _total_invokes{0};
      
    public:
      MethodStats();
      explicit MethodStats(std::string desc);
      MethodStats& operator++();
      const std::string& descriptor() const;
      int invoked_count() const;
      const static std::string TAG;
      const static std::string TAG_END;

    };
    int operator<(const MethodStats& m1, const MethodStats& m2);
    MethodStats& operator>>(const std::string&, MethodStats& ref);
    std::vector<MethodStats>& operator>>(const std::string&, std::vector<MethodStats>& ref);

    template <typename OutType>
      OutType& operator<<(OutType& out, const MethodStats& ref)
      {
        out << ref.descriptor() << "<end-descriptor>" << ref.invoked_count();
        return out;
      }

    template <typename OutType>
      OutType& operator<<(OutType& out, const std::vector<MethodStats>& ref)
      {
        if(ref.size() == 0) return out;
        out << MethodStats::TAG << "<view>";
        for(auto& x : ref) {
          out << x << MethodStats::TAG_END;
        }
        return out;
      }

    
  }
}


#endif
