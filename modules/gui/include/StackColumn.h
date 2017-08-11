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

#ifndef STACKCOL_H
#define STACKCOL_H


#include <unordered_map>
#include <unordered_set>
#include "StackTrace.h"

namespace frenchroast {
  using namespace frenchroast::monitor;
  

class StackColumn {

  int _column;
  int _totalRows;
  std::unordered_map<std::string, int> _stackline;
  std::vector<StackTrace>               _stacks;
  std::unordered_set<std::string>       _allkeys;

 public:
  StackColumn(int col);
  StackColumn();
  int  column();
  bool update(const StackTrace&);
  int  required_rows();
  std::vector<StackTrace>&               stacks();
};
}
#endif
