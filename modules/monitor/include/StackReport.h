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

#ifndef STACKREPORT_H
#define STACKREPORT_H
#include <string>
#include "StackTrace.h"

namespace frenchroast { namespace monitor {
    class StackReport {
      friend     StackReport& operator>>(const std::string&, StackReport& ref);
      std::size_t          _count{0};
      StackTrace           _trace;

    public:
      StackReport(const StackTrace&);
      StackReport();
      StackReport&             operator++();
      StackReport&             operator+=(const StackReport&);
      int                      count() const;
      const std::string&       key() const;
      const StackTrace&        trace() const;
      std::vector<StackFrame>  descriptors() const;
      bool                     operator==(const StackReport&) const;
    };
    
    template <typename OutType>
      OutType& operator<<(OutType& out, const StackReport& ref)
      {
        out << ref.count() << "<end-count>" << ref.trace();
        return out;
      }

    StackReport& operator>>(const std::string&, StackReport& ref);    
  }
}


#endif
