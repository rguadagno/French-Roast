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

  std::vector<std::vector<std::string>>& StackColumn::stacks() 
  {
    return _stacks;
  }
  

  bool StackColumn::update(const StackTrace& trace)
  {
    if(_allkeys.count(trace.key()) == 1) return false;  
    _allkeys.insert(trace.key());
    std::stack<std::string> skey = trace.keys();
    while(!skey.empty()) {
      if(_stackline.count(skey.top()) == 1) {
        _stackline[trace.key()] =  _stackline[skey.top()];
        _stackline.erase(skey.top());
        std::swap(_stacks[_stackline[trace.key()]], trace.frames());
        return true;;
      }
      skey.pop();
    }
    if(_stackline.count(trace.key()) == 0) {
      _stacks.push_back(trace.frames());
      _stackline[trace.key()] = _stacks.size() - 1;
      return true;
    }
    return false; // do assert...
  }
  
}

