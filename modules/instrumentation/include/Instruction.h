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
#ifndef INSTRUCTION_H
#define INSTRUCTION_H
#include "OpCode.h"
#include "Util.h"

namespace frenchroast {
  class Instruction {
    OpCode _opCode;
    int    _address;
    BYTE   _operand[2];
    BYTE*  _opbuf;
    int    _opbuf_size;
    static const  int PAD_MAX = 3;

    int load_dynamic_from_buffer(const BYTE* buf, int address);
    int load_fixed_from_buffer(const BYTE* buf, int address, int& loaded);
  public:
    Instruction();
    Instruction(const OpCode& opcode);
    Instruction(const OpCode& opcode, short operand);
    ~Instruction();
    Instruction(Instruction&& ref);
    void operator=( Instruction& ref);
    const std::string get_name() const;
    int size() const;
    int load_from_buffer(const BYTE* buf, int address, int& loaded);
    int load_to_buffer(BYTE* buf);
    short get_operand() const;
    short offset() const;
    void set_operand(short operand);
    int address();
    OpCode& opcode();
    void set_address(int addr);
    bool is_branch() const;
    void adjust(int insertedAt);
    BYTE* get_buffer();
    operator BYTE() const;
    static int calc_pad(int startAddress);
  };
    
  std::ostream& operator<<(std::ostream& out, const Instruction& ref);
}
#endif
