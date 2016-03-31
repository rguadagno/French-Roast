// copyright (c) 2016 Richard Guadagno
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
#ifndef FRMETHOD_H
#define FRMETHOD_H
#include <iostream>
#include <vector>
#include "Instruction.h"
#include "StackMapFrame.h"

namespace frenchroast {
  class Method {  // inflated from raw bytes
    short _maxStack;
    short _maxLocals;
    int   _codeLength;
    std::vector<Instruction> _instructions; 
  public:
    void load_from_buffer(const BYTE* buf);
    void load_to_buffer(BYTE* buf);
    int get_max_stack() const;
    void set_max_stack(int v);
    int get_max_locals() const;
    int size_in_bytes() const;
    void add_instructions(int insertAt, std::vector<Instruction>& ilist, bool branchafter);
    void adjust_frames(std::vector<StackMapFrame*>& frames);
    Instruction& operator[](int idx);
  };

  std::ostream& operator<<(std::ostream& out, const Method& ref);
}
#endif
