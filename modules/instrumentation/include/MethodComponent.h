// copyright (c) 2018 Richard Guadagno
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
//    along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
//
#ifndef FRMETHODCOMP_H
#define FRMETHODCOMP_H
#include <iostream>
#include <unordered_map>
#include <vector>
#include "MethodInfo.h"
#include "ConstantPool.h"

namespace frenchroast {

  class MethodComponent : public ClassFileComponent { 

    std::unordered_map<std::string, MethodInfo> _methods;
    ConstantPoolComponent&                      _pool;
  public:
    MethodComponent(ConstantPoolComponent& pool);
    int  size_in_bytes() const;
    void reset();
    void load_from_buffer(const BYTE* buf);
    void load_to_buffer(BYTE* buf);
    void display(std::ostream& out);
    std::vector<std::string> get_method_descriptors();
    void add_method( MethodInfo&& method);
    bool has_method(const std::string&) const;
    MethodInfo& get_method(const std::string&);
  };

}
#endif
