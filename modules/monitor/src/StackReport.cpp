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

#include "StackReport.h"
#include "Util.h"

namespace frenchroast { namespace monitor {

    StackReport::StackReport(const StackTrace& trace) : _trace(trace), _count(1)
    {
    }

    StackReport::StackReport()
    {
    }
    
    StackReport& StackReport::operator++() {
      ++_count;
      return *this;
    }

    StackReport& StackReport::operator+=(const StackReport& ref)
    {
      if(_count == 0) {
        _trace = ref._trace;
      }
      ++_count;      
      return *this;
    }
    
    int StackReport::count() const
    {
      return _count;
    }

    const std::string& StackReport::key() const
    {
      return _trace.key();
    }

    const StackTrace& StackReport::trace() const
    {
      return _trace;
    }

    bool StackReport::operator==(const StackReport& ref) const
    {
      return _count == ref._count && _trace == ref._trace;
    }

    std::vector<StackFrame> StackReport::descriptors() const
    {
      return _trace.descriptor_frames();
    }
    
    StackReport& operator>>(const std::string& rep, StackReport& ref)
    {
      auto parts = frenchroast::split(rep, "<end-count>");
      ref._count = atoi(parts[0].c_str());
      parts[1] >> ref._trace;
      return ref;
    }
  
    
  }
}
