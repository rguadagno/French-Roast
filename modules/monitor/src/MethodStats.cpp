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

#include "MethodStats.h"
#include <string>

namespace frenchroast { namespace monitor {
    MethodStats::MethodStats()
    {
    }

    MethodStats::MethodStats(std::string desc) : _descriptor(desc)
    {
    }
    
    const std::string& MethodStats::descriptor() const
    {
      return _descriptor;
    }

    int MethodStats::invoked_count() const 
    {
      return _total_invokes;
    }  
    
    
    //MethodStats(std::string desc);
    
    MethodStats& MethodStats::operator++()
    {
      ++_total_invokes;
      return *this;
    }
   
     int operator<(const MethodStats& m1, const MethodStats& m2)
     {
       return m2.invoked_count() < m1.invoked_count();
     }


}
}
