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


namespace frenchroast { namespace monitor {

    StackReport::StackReport(std::vector<std::string> descriptors) : _descriptors(descriptors), _count(1)
    {
      _key = "";
      for(auto& x : _descriptors) {
        _key.append(x);
      }
    }

    StackReport::StackReport()
    {
    }
    
    StackReport& StackReport::operator++() {
      ++_count;
      return *this;
    }
    int StackReport::count() const
    {
      return _count;
    }

    const std::string& StackReport::key() const
    {
      return _key;
    }

    const std::vector<std::string>& StackReport::descriptors() const
    {
      return _descriptors;
    }
  }
}
