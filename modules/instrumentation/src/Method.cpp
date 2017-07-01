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
#include "Method.h"
#include "OpCodeConst.h"
#include "FrameConst.h"
#include <set>


namespace frenchroast {
  void Method::load_from_buffer(const BYTE* buf,const BYTE* excepbuf)
  {
    _instructions.clear();
    _maxStack   = to_int(buf, 2);
    _maxLocals  = to_int(buf + 2, 2);
    _codeLength = to_int(buf + 4, 4);
    Instruction instr;
    int totalLen = 0;
    int loaded;
    int address = 0;
    while (totalLen < _codeLength) {
      address += instr.load_from_buffer(buf+8+address,address,loaded);
      if (!loaded ) {
        break;
         }
      totalLen += instr.size();
      _instructions.push_back(std::move(instr));
    }
    _exceptions.clear();
    Exception excep;
    int excepCount = to_int(excepbuf,2);
    for(int idx = 0; idx < excepCount;idx++) {
      excep.load_from_buffer(excepbuf + 2 + 8*idx);
      _exceptions.push_back(std::move(excep));
    }
  }

  void Method::load_to_buffer(BYTE* buf,BYTE* excepbuf)
  {
    int offset = 0;
    write_big_e_bytes(buf + offset, &_maxStack);
    offset += sizeof(_maxStack);
    write_big_e_bytes(buf + offset, &_maxLocals);
    offset += sizeof(_maxLocals);
    write_big_e_bytes(buf + offset, &_codeLength);
    offset += sizeof(_codeLength);
    for(auto& x : _instructions) {
      offset += x.load_to_buffer(buf + offset);
    }

    write_bytes(excepbuf, _exceptions.size(),2);
    int idx = 0;
    for(auto& x: _exceptions) {
      x.load_to_buffer(excepbuf + 2 + idx*8);
      ++idx;
    }
  }

  int Method::get_max_stack() const
  {
    return _maxStack;
  }

  void Method::set_max_stack(int v)
  {
    _maxStack = v;
  }

  int Method::get_max_locals() const
  {
    return _maxLocals;
  }

  int Method::size_in_bytes() const
  {
    return sizeof(_maxStack) + sizeof(_maxLocals) + sizeof(_codeLength) + _codeLength;
  }


  std::vector<int> get_targets(Instruction& inst)
  {
    std::vector<int> rv;
    if ( !inst.is_branch()) {
      return rv;
    }
    if (inst.opcode() == opcode::lookupswitch) {
      int pairs = to_int(inst.get_buffer() + Instruction::calc_pad(inst.address())+4,4);
      rv.push_back(inst.address() +   to_int(inst.get_buffer() + Instruction::calc_pad(inst.address()),4));
      for(int idx = 0; idx < pairs; idx++) {
        rv.push_back( inst.address() + to_int(inst.get_buffer() + Instruction::calc_pad(inst.address())+ 8 + 4 + idx * 8 ,4)   );
      }

      return rv;
    }
    if (inst.opcode() == opcode::tableswitch) {
      return rv; //@@ not implemented yet
    }
    rv.push_back(inst.address() + inst.offset());
    return rv;
  }

  void  update_exception(Exception& excep, std::unordered_map<int,int> origAddr, std::unordered_map<int,int> newAddr)
  {
    excep.set_start_pc(newAddr[origAddr[excep.get_start_pc()]]);
    excep.set_end_pc(newAddr[origAddr[excep.get_end_pc()]]);
    excep.set_handler_pc(newAddr[origAddr[excep.get_handler_pc()]]);
  }
  
  void update_targets(Instruction& inst, std::unordered_map<int,int> origAddr, std::unordered_map<int,int> newAddr, std::vector<int>& origTargets, int& nextTarget)
  {
    if ( !inst.is_branch()) {
      return;
    }
    if (inst.opcode() == opcode::lookupswitch) {

      int intendedTarget = newAddr[origAddr[origTargets[nextTarget]]];
      int defaultoffset = to_int(inst.get_buffer() + Instruction::calc_pad(inst.address()),4);
      int target = inst.address() + defaultoffset;
      ++nextTarget;
      int offsetChange = intendedTarget - target;
      write_bytes(inst.get_buffer() + Instruction::calc_pad(inst.address()),  defaultoffset+ offsetChange,4);
      int pairs = to_int(inst.get_buffer() + Instruction::calc_pad(inst.address())+4,4);
      for(int idx = 0; idx < pairs; idx++) {
         BYTE* offsetPtr = inst.get_buffer() + Instruction::calc_pad(inst.address())+ 8 + 4 + idx * 8 ;
         int intendedTarget = newAddr[origAddr[origTargets[nextTarget]]];
         ++nextTarget;
         int target = inst.address() + to_int(offsetPtr,4);
         int offsetChange = intendedTarget - target;
         write_bytes(offsetPtr , to_int(offsetPtr,4) + offsetChange,4);
      }
      return;
    }
    if (inst.opcode() == opcode::tableswitch) {
      return; //@@ not implemented yet
    }
    int intendedTarget = newAddr[ origAddr[origTargets[nextTarget]]];
    ++nextTarget;
    int target = inst.address() + inst.offset();
    int offsetChange =  intendedTarget - target;
    inst.set_operand(inst.offset() + offsetChange);
      
  }
  
  void Method::add_instructions(int insertAt, std::vector<Instruction>& ilist, bool sticky)
  {
    int totalbytes=0;
    for (auto& x : ilist) {
      totalbytes += x.size();
    }

    // collect info
    std::unordered_map<int,int> origAddr;
    std::unordered_map<int,int> newAddr;
    std::vector<int> origTargets;
    int instIdx = 0;
    for (auto& x : _instructions) {
      origAddr[x.address()] = ++instIdx;
      if (x.is_branch()) {
        for (auto& target : get_targets(x)) {
          origTargets.push_back(target);
         }
      }
    }

    // insert and re-calc addr
    std::vector<Instruction> newvec;
    instIdx = 0;
    int nextAddr=0;
    newvec.reserve(ilist.size() + _instructions.size());
    for (auto& x : _instructions) {
       if (x.address() == insertAt) {
          for (auto& instruc : ilist) {
            instruc.set_address(nextAddr);
            nextAddr += instruc.size();
            newvec.push_back(std::move(instruc));
          }
       }
       
       x.adjust(nextAddr);
       int oldaddr = x.address();
       x.set_address(nextAddr);
       if (sticky) {
          newAddr[++instIdx] = x.address();
       }
       else {
          newAddr[++instIdx] = oldaddr;
       }
       nextAddr += x.size();
       newvec.push_back(std::move(x));
    }
    _instructions = std::move(newvec);


    // update branch offsets
    int idx = 0;
    for (auto& x : _instructions) {
      if ( x.is_branch()) {
        update_targets(x, origAddr, newAddr, origTargets,idx);
      }
    }

    for (auto& x : _exceptions) {
      update_exception(x, origAddr, newAddr);
    }

    
    _codeLength = 0;
    for(auto& x : _instructions) {
      _codeLength += x.size();
    }
  }

  void Method::adjust_frames(std::vector<StackMapFrame*>& frames)
  {
    // collect in a set unique targets, sorted in asced order
    std::set<int> targets;
    for(auto& x : _instructions) {
      if( x.is_branch()) {
        for(auto& tg : get_targets(x)) {
          targets.insert(tg);
        }
      }
    }
    for(auto& x : _exceptions) {
      targets.insert(x.get_handler_pc());
    }

    if (frames.size() != targets.size()) {
      std::cout << "ERROR: adjust_frames: targets [ " << targets.size() << " ] / frames [ " << frames.size() << " ] count does not match" << std::endl;
      
      exit(0);
    }

    std::vector<int> tlist;
    for(auto& x : targets) {
      tlist.push_back(x);
    }
    
    int running = 0;
    int idx =  0;
    int sub = 0;
    for(auto& ptr : frames) {
      int offset = tlist[idx] - running;
      if (idx > 0) {
        sub = 1;
      }
      
      if (!ptr->adjust_offset(offset - sub)) {
        if (*ptr == stackmapframe::same) {
          ptr = new SameFrameExtended();
          ptr->adjust_offset(offset - sub);
        }
      }
      running += offset;
      ++idx;
    }

  }
                        

  Instruction& Method::operator[](int idx)
  {
    return _instructions[idx-1];
  }

  std::ostream& operator<<(std::ostream& out, const Method& ref)
  {
    out << "SIZE(BYTES):" << ref.size_in_bytes() << "   max stack=" << ref.get_max_stack() << "   locals=" << ref.get_max_locals();
    return out;
  }


  std::vector<int> Method::get_return_addresses()
  {
    std::vector<int> rv;

    int total = _instructions.size() -1;
    for (int idx = total;idx >=0;idx--) {
      if (_instructions[idx] == opcode::xreturn) {
        rv.push_back(_instructions[idx].address());
      }
      if (_instructions[idx] == opcode::ireturn ||
           _instructions[idx] == opcode::lreturn ||
          _instructions[idx] == opcode::freturn ||
          _instructions[idx] == opcode::dreturn ||
          _instructions[idx] == opcode::areturn 
          ) {
        rv.push_back(_instructions[idx-1].address());
      }

    }
    
    return rv;
  }

  
}
