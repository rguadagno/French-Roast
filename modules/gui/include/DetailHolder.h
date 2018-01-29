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

#ifndef DETAILHOLDER_H
#define DETAILHOLDER_H

#include <unordered_map>
#include "StackReport.h"
#include "MarkerField.h"

using namespace frenchroast::monitor;
class DetailHolder {
 public:
 DetailHolder( std::size_t count,
               std::vector<std::string> ah,
               std::vector<std::string> ih,
               const std::unordered_map<std::string, MarkerField>& fields,
               const std::unordered_map<std::string, StackReport>& stacks) : _count(count),
                                                                      _argHeaders(ah),
                                                                      _instanceHeaders(ih),
                                                                       _markers(fields),
                                                                      _stacks(stacks)
    {
    }

  DetailHolder()
    {
    }
  
  std::size_t                                  _count;
  std::vector<std::string>                     _argHeaders;
  std::vector<std::string>                     _instanceHeaders;
  std::unordered_map<std::string, MarkerField> _markers;
  std::unordered_map<std::string, StackReport> _stacks;
};

#endif
