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

#include <iostream>
#include <string>
#include "Instruction.h"
#include "OpCodeConst.h"

namespace frenchroast {
  
  Instruction::Instruction()  : _opbuf(nullptr)
  {
  }

  Instruction::Instruction(const OpCode& opcode) : _opCode(opcode), _opbuf(nullptr)
  {
  }
  
  Instruction::Instruction(const OpCode& opcode, short operand) : _opCode(opcode), _opbuf(nullptr)
  {
    write_big_e_bytes(_operand,&operand);
  }

  Instruction::~Instruction()
  {
    if (_opbuf != nullptr) {
      delete[] _opbuf;
    }
  }

  Instruction::Instruction( const Instruction& ref)
  {
    _opCode = ref._opCode;
    _address = ref._address;
    memcpy(_operand, ref._operand, 2);
    _opbuf_size = ref._opbuf_size;
    if ( _opCode.is_dynamic() || _opCode.is_raw()) {
      _opbuf = new BYTE[_opbuf_size];
      memcpy(_opbuf,ref._opbuf,_opbuf_size);

    }
    else {
      _opbuf = nullptr;
    }
  }

  
  Instruction::Instruction( Instruction&& ref)
  {
    _opCode = ref._opCode;
    _address = ref._address;
    memcpy(_operand, ref._operand, 2);
    _opbuf_size = ref._opbuf_size;
    if ( _opCode.is_dynamic() || _opCode.is_raw()) {
      _opbuf = ref._opbuf;
      ref._opbuf = nullptr;
    }
    else {
      _opbuf = nullptr;
    }
  }

  void Instruction::operator=( const Instruction& ref)
  {
    delete _opbuf;
    _opCode = ref._opCode;
    _address = ref._address;
    memcpy(_operand, ref._operand, 2);
    _opbuf_size = ref._opbuf_size;
    if ( _opCode.is_dynamic() || _opCode.is_raw()) {
      _opbuf = new BYTE[_opbuf_size];
      memcpy(_opbuf,ref._opbuf,_opbuf_size);
    }
    else {
      _opbuf = nullptr;
    }
  }
  
  const std::string Instruction::get_name() const
  {
    return _opCode.get_name();
  }
    
  int Instruction::size() const
  {
    return _opCode.is_dynamic() ? 1 + _opbuf_size : _opCode.get_size();
  }


  int Instruction::calc_pad(int startAddress) 
  {
    return 4 - (startAddress % 4 + 1);
  }
  
  int Instruction::load_from_buffer(const BYTE* buf, int address, int& loaded)
  {
    _address = address;
    loaded = 1;
    _opCode = _opCode[*buf];
    _opbuf = nullptr;
    if (_opCode.is_dynamic()) {
      return load_dynamic_from_buffer(buf, address );
    }
    else {
      return load_fixed_from_buffer(buf, address, loaded );
    }
  }

  int Instruction::load_fixed_from_buffer(const BYTE* buf, int address, int& loaded)
  {
    memset(_operand,0,2);

    if(_opCode.is_raw()) {
      _opbuf_size = _opCode.get_size() -1;
      _opbuf = new BYTE[_opbuf_size];
      memcpy(_opbuf, buf + 1, _opbuf_size );
      return _opCode.get_size();
    }
    
    if (_opCode.get_size() == 0) {
      loaded=0;
    }
    if (_opCode.get_size() == 3) {
      memcpy(&_operand[0],buf+1,2);
    }
    if(_opCode.get_size() == 2) {
      memcpy(&_operand[1],buf+1,1);
    }
    return _opCode.get_size();
  }

  int Instruction::load_dynamic_from_buffer(const BYTE* buf, int address)
  {
    if (_opCode.get_code() == opcode::lookupswitch)
      {
        _opbuf_size = calc_pad(_address);
        _opbuf_size += 4;
        _opbuf_size += (8 * to_int(buf + 1 + _opbuf_size, 4));
        _opbuf_size += 4;
      }
    else if (_opCode.get_code() == opcode::tableswitch)
      {
        int pad = calc_pad(_address);
        _opbuf_size = pad + 4 + 4 + 4;
        int low = to_int(buf + 1 + pad + 4,4);
        int high = to_int(buf + 1 + pad + 4 + 4,4);
        int offsets = (high - low) + 1;
        _opbuf_size += 4 * offsets;
      }
    _opbuf = new BYTE[_opbuf_size];
    memcpy(_opbuf, buf + 1, _opbuf_size);
    return 1 + _opbuf_size;
  }
  
  int Instruction::load_to_buffer(BYTE* buf)
  {
    *buf = _opCode.get_code();
    if (_opCode.is_dynamic()) {
      memcpy(buf + 1, _opbuf, _opbuf_size);
    }
    else {
      if(_opCode.is_raw()) {
        memcpy(buf + 1, _opbuf, _opbuf_size);
      }
      else if (_opCode.get_size()==3) {
        memcpy(buf + 1,&_operand[0], 2);
      }
      else if (_opCode.get_size()==2) {
         memcpy(buf + 1,&_operand[1], 1);
      }
    }
    return size();
  }


  void Instruction::adjust(int newAddr)
  {
    if (_opCode.get_code() != opcode::lookupswitch && _opCode.get_code() != opcode::tableswitch)  {
      return;
    }
    int currentPad = calc_pad(_address);
    int newPad = calc_pad(newAddr);
    int adjustPad = currentPad - newPad;
    if (adjustPad != 0) {
      BYTE* newbuf = new BYTE[_opbuf_size - adjustPad];
      memset(newbuf,0, _opbuf_size - adjustPad);
      memcpy(newbuf + newPad, _opbuf + currentPad,_opbuf_size-currentPad);
      _address = newAddr;
      _opbuf_size = _opbuf_size - adjustPad;
      
      delete[] _opbuf;
      _opbuf = newbuf;
      int pad = calc_pad(_address);
      int defaultoffset = to_int(_opbuf + pad,4 ) + adjustPad * -1;
      write_big_e_bytes(_opbuf + pad, &defaultoffset);
      if (_opCode.get_code() == opcode::lookupswitch) {
        int totalPairs = to_int(_opbuf + pad + 4,4 );
        for(int idx = 0; idx < totalPairs; idx++) {
          defaultoffset = to_int(_opbuf + pad + 8 + (8 * idx) + 4,4 ) + adjustPad * -1;
          write_big_e_bytes(_opbuf + pad + 8 + (8 * idx) + 4, &defaultoffset);
        }
      }
      if ( _opCode.get_code() == opcode::tableswitch) {
        int low = to_int(_opbuf  + pad + 4,4);
        int high = to_int(_opbuf  + pad + 4 + 4,4);
        int offsets = (high - low) + 1;
        for(int idx = 0; idx < offsets; idx++) {
          defaultoffset = to_int(_opbuf + pad + 8 + (4 * idx) + 4,4 ) + adjustPad * -1;
          write_big_e_bytes(_opbuf + pad + 8 + (4 * idx) + 4, &defaultoffset);
        }
      }
    }
  }


  BYTE* Instruction::get_buffer()
  {
    return _opbuf;
  }
  
  short Instruction::get_operand() const
  {
    if(_opCode.get_size() == 1) {
      return 0;
    }
    return  (_operand[0] << 8) | _operand[1];
  }

  short Instruction::offset() const
  {
    return is_branch() ? get_operand() : 0;
  }
  
  void Instruction::set_operand(short operand)
  {
    write_big_e_bytes(_operand,&operand);
  }
  
  int Instruction::address() const
  {
    return _address;
  }

  OpCode& Instruction::opcode()
  {
    return _opCode;
  }
  
  void Instruction::set_address(int addr)
  {
    _address = addr;
  }

  bool Instruction::is_branch() const
  {
    return _opCode.is_branch();
  }

  Instruction::operator BYTE() const
  {
    return _opCode;
  }
  
  std::ostream& operator<<(std::ostream& out, const Instruction& ref)
  {
    out << ref.get_name() << "  " << ref.size();
    return out;
  }
}
