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

#include "StackColumn.h"
#include <iostream>


namespace frenchroast {


  StackColumn::StackColumn(int col) : _column(col)
  {
  }


  StackColumn::StackColumn()
  {
  }

  int StackColumn::column()
  {
    return _column;
  }

  int StackColumn::required_rows()
  {
    int rv = 0;
    for(auto& line : _stacks) {
      rv += line.size();
    }
    return rv;
  }

  std::vector<StackTrace>& StackColumn::stacks() 
  {
    return _stacks;
  }

  bool StackColumn::update(const StackTrace& trace)
  {
    bool rv = false;
    bool matched = false;
    for(auto& ctrace  : _stacks) {
      
      if(ctrace == trace) {
          matched = true;
         rv = ctrace.update_monitors(trace);
      }
      if(ctrace > trace) {
        matched = true;
       rv =  ctrace.update_monitors(trace);
      }
      if(ctrace < trace) {
       ctrace = trace;
      return true;
      }
    }

    if(!matched) {
      rv =true;
      _stacks.push_back(trace);
    }
    return rv;
  }

  
}

