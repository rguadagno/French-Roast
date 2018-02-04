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


#include <set>
#include "MethodInfo.h"
#include "OpCodeConst.h"
#include "FrameConst.h"

namespace frenchroast {
  
  MethodInfo::MethodInfo(short flags,short name_index,short descriptor_index) : _access_flags(flags), _name_index(name_index), _descriptor_index(descriptor_index)
    {
    }


  MethodInfo& MethodInfo::operator=(MethodInfo&& ref)
  {
    _access_flags           = ref._access_flags;
    _name_index             = ref._name_index;
    _descriptor_index       = ref._descriptor_index;
    _attributes_count       = ref._attributes_count;
    _max_stack              = ref._max_stack;
    _max_locals             = ref._max_locals;
    _code_length            = ref._code_length;
    _exception_table_length = ref._exception_table_length;
    _code_attributes_count  = ref._code_attributes_count;
    _exceptions             = ref._exceptions;
    _attributes             = std::move(ref._attributes);
    _codeAttributes         = std::move(ref._codeAttributes);
    _instructions           = std::move(ref._instructions);

    
    for(auto& ptr : _frames) {
      delete ptr;
    }
    _frames.clear();
    _frames = std::move(ref._frames);

    ref._frames.clear();
    return *this;
  }

  MethodInfo::MethodInfo(const MethodInfo& ref)
  {
    _access_flags           = ref._access_flags;
    _name_index             = ref._name_index;
    _descriptor_index       = ref._descriptor_index;
    _attributes_count       = ref._attributes_count;
    _max_stack              = ref._max_stack;
    _max_locals             = ref._max_locals;
    _code_length            = ref._code_length;
    _exception_table_length = ref._exception_table_length;
    _code_attributes_count  = ref._code_attributes_count;
    _exceptions             = ref._exceptions;
    _attributes             = ref._attributes;
    _codeAttributes         = ref._codeAttributes;
    _instructions           = ref._instructions;
    _frames.clear();
    for(auto& ptr : ref._frames) {
      _frames.push_back(ptr->copy());
    }
  }

  
  MethodInfo::~MethodInfo()
  {
    
    for(StackMapFrame* frame : _frames) {
      delete frame;
     }
  }
  
  std::vector<int> MethodInfo::get_return_addresses() const
  {
    std::vector<int> rv;
    std::size_t total =  _instructions.size() -1;
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
  
  short MethodInfo::get_max_stack() const
  {
    return _max_stack;
  }
  
  void MethodInfo::set_max_stack(short maxstack)
  {
    _max_stack = maxstack;
  }

  void MethodInfo::add_instructions(int insertAt,  std::vector<Instruction>& ilist, bool sticky)
  {
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
    int adjustAfter = 0;
    newvec.reserve(ilist.size() + _instructions.size());
    for (auto& x : _instructions) {
       if (x.address() == insertAt) {
          for (auto& instruc : ilist) {
            instruc.set_address(nextAddr);
            if(instruc.is_branch()) {
              adjustAfter = instruc.address();
            }
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

    _code_length = 0;
    for(auto& inst : _instructions) {
      _code_length += inst.size();
    }

    // update branch offsets
    int idx = 0;
    if(origTargets.size() > 0) {
      for (auto& x : _instructions) {
        if ( x.is_branch() && x.address() > adjustAfter) {
          update_targets(x, origAddr, newAddr, origTargets,idx);
        }
      }
    }

    for (auto& x : _exceptions) {
      update_exception(x, origAddr, newAddr);
    }
    adjust_frames(_frames);
  }

  std::vector<int> MethodInfo::get_targets(Instruction& inst)
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


  void MethodInfo::update_targets(Instruction& inst, std::unordered_map<int,int> origAddr, std::unordered_map<int,int> newAddr, std::vector<int>& origTargets, int& nextTarget)
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
    //    if(nextTarget + 1 > origTargets.size() )return;
    int intendedTarget = newAddr[ origAddr[origTargets[nextTarget]]];
    ++nextTarget;
    int target = inst.address() + inst.offset();
    int offsetChange =  intendedTarget - target;
    inst.set_operand(inst.offset() + offsetChange);
      
  }

  void  MethodInfo::update_exception(Exception& excep, std::unordered_map<int,int> origAddr, std::unordered_map<int,int> newAddr)
  {
    excep.set_start_pc(newAddr[origAddr[excep.get_start_pc()]]);
    excep.set_end_pc(newAddr[origAddr[excep.get_end_pc()]]);
    excep.set_handler_pc(newAddr[origAddr[excep.get_handler_pc()]]);
  }

  std::vector<StackMapFrame*> MethodInfo::frames()
  {
    return _frames;
  }
  
  Instruction& MethodInfo::operator[](int idx)
  {
    return _instructions[idx-1];
  }

  int MethodInfo::size_in_bytes() const
  {
    int rv= sizeof(_access_flags) +
            sizeof(_name_index) +
            sizeof(_descriptor_index) +
            sizeof(_attributes_count);
    if(_attributes_count == 0) return rv;
    for(auto& attr : _attributes) {
      if(attr.first == attribute::Code ) continue;
      rv += attr.second.size_in_bytes();
    }
    for(auto& attr : _codeAttributes) {
      if(attr.first == attribute::StackMapTable) continue;
      rv += attr.second.size_in_bytes();
    }
    rv += attribute::HeaderSize; // for Code

    if(_frames.size() > 0) {
      rv += attribute::HeaderSize + attribute::CountSize; // for StackMapTable
    }

    for(auto& frame : _frames) {
      rv += frame->size_in_bytes();
    }

    rv += _code_length;
    rv += sizeof(_max_stack);
    rv += sizeof(_max_locals);
    rv += sizeof(_code_length);
    rv += sizeof(_exception_table_length);
    rv += sizeof(_code_attributes_count);
    rv += _exception_table_length * attribute::ExceptionSize;
    return rv;
  }

  void MethodInfo::load_attributes_from_buffer(BYTE*& ptr, short count, std::unordered_map<std::string, Attribute<std::string>>& dest, std::unordered_map<int,std::string>& names)
  {
    for(short idx = 0; idx < count; idx++) {
      Attribute<std::string> attrib;
      attrib.load_from_buffer(ptr);
      ptr += attrib.size_in_bytes();
      dest[names[attrib.get_name_index()]] = std::move(attrib);
    }
  }

  void MethodInfo::load_attributes_to_buffer(BYTE*& ptr, std::unordered_map<std::string, Attribute<std::string>>& src)
  {
    for(auto& attr : src) {
      if(attr.first == attribute::Code || attr.first == attribute::StackMapTable) continue;
      attr.second.load_to_buffer(ptr);
      ptr += attr.second.size_in_bytes();
    }
  }

  
  int MethodInfo::load_from_buffer(const BYTE* buf,  std::unordered_map<int,std::string>& names)
  {
    BYTE* ptr = const_cast<BYTE*>(buf);
    _instructions.clear();

    to_value(ptr, _access_flags);
    to_value(ptr, _name_index);
    to_value(ptr, _descriptor_index);
    to_value(ptr, _attributes_count);
    if(_attributes_count == 0) return size_in_bytes();
    load_attributes_from_buffer(ptr,_attributes_count,_attributes,names);
    ptr = _attributes[attribute::Code]._info;
    to_value(ptr, _max_stack);
    to_value(ptr, _max_locals);
    to_value(ptr, _code_length);
    Instruction instr;
    int totalLen = 0;
    int loaded;
    int address = 0;
    while (totalLen < _code_length) {
      address += instr.load_from_buffer(ptr + address,address,loaded);
      if (!loaded ) {
        break;
         }
      totalLen += instr.size();
      _instructions.push_back(std::move(instr));
    }
    ptr += address;
    _exceptions.clear();
    Exception excep;
    to_value(ptr, _exception_table_length);
    for(int idx = 0; idx < _exception_table_length;idx++) {
      excep.load_from_buffer(ptr + attribute::ExceptionSize * idx);
      _exceptions.push_back(std::move(excep));
    }
    
    ptr += attribute::ExceptionSize * _exception_table_length;

 
    to_value(ptr, _code_attributes_count);
    load_attributes_from_buffer(ptr,_code_attributes_count,_codeAttributes,names);
    if(_codeAttributes.find(attribute::StackMapTable) != _codeAttributes.end()) {
      _frames = load_frames_from_buffer(to_int(_codeAttributes[attribute::StackMapTable]._info,2),
                                        _codeAttributes[attribute::StackMapTable]._info + 2);
    }
    return size_in_bytes();
  }


  void show_buffs(BYTE* xbuf, BYTE* origbuf, int total)
  {
    std::cout << "============== bufs @ " << total << " ==================" << std::endl;
    std::cout << "new: ";
    for(int x = 0; x < total;x++)
      std::cout << std::hex << (int)xbuf[x];
    std::cout << std::endl;
    std::cout << "orig:";
    for(int x = 0; x < total;x++)
      std::cout << std::hex << (int)origbuf[x];
    std::cout << std::endl;
    std::cout << "=========================================================" << std::endl;
  }

  int MethodInfo::load_to_buffer(BYTE* buf, std::unordered_map<std::string, int>& ids)
  {
    BYTE* xbuf = buf;
    wbytes(buf, &_access_flags);
    wbytes(buf, &_name_index);
    wbytes(buf, &_descriptor_index);
    wbytes(buf, &_attributes_count);

    if(_attributes_count == 0) return size_in_bytes();
    short count = 0;
    int stackmapAttrSize = _frames.size() > 0 ? sizeof(count)  : 0;
    for(auto& ptr : _frames) {
       stackmapAttrSize += ptr->size_in_bytes();
    }
    if(_frames.size() > 0) {
      count = 1;
    }

    int codeAttrLen = stackmapAttrSize;
    for(auto& attr : _codeAttributes) {
      if(attr.first == attribute::StackMapTable) continue;
      codeAttrLen += attr.second.get_length();
      codeAttrLen += attribute::HeaderSize;
      ++count;
    }
    if(_frames.size() > 0) {
      codeAttrLen += attribute::HeaderSize;
    }
    
    short codeid = ids[attribute::Code];
    wbytes(buf, &codeid);

    int attrLen = sizeof(_max_stack) + sizeof(_max_locals) + sizeof(_code_length) + _code_length
      + sizeof(_exception_table_length) + _exception_table_length * attribute::ExceptionSize + attribute::CountSize +
      codeAttrLen;

    wbytes(buf, &attrLen);
    wbytes(buf, &_max_stack);
    wbytes(buf, &_max_locals);
    wbytes(buf, &_code_length);

    for(auto& inst : _instructions) {
      buf += inst.load_to_buffer(buf);
    }
    wbytes(buf, &_exception_table_length);

    int idx = 0;
    for(auto& excep : _exceptions) {
      excep.load_to_buffer(buf + attribute::ExceptionSize * idx++);
    }
    buf += attribute::ExceptionSize * _exception_table_length;
    wbytes(buf, &count);
    short framesCount = _frames.size();
    if(_frames.size() > 0) {
      short smid = ids[attribute::StackMapTable];
      wbytes(buf, &smid);
      wbytes(buf, &stackmapAttrSize);
      wbytes(buf, &framesCount);
      for(auto& ptr : _frames) {
        ptr->load_to_buffer(buf);
        buf += ptr->size_in_bytes();
      }
    }

    load_attributes_to_buffer(buf, _codeAttributes);
    load_attributes_to_buffer(buf, _attributes);
    return size_in_bytes();
  }


  
  void MethodInfo::adjust_frames(std::vector<StackMapFrame*>& frames)
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

  void MethodInfo::set_name_index(short idx)
  {
    _name_index = idx;
  }
  
  void MethodInfo::set_descriptor_index(short idx)
  {
    _descriptor_index = idx;
  }
  
  void MethodInfo::set_access_flags(short flags)
  {
    _access_flags = flags;
  }

  short MethodInfo::get_name_index() const
  {
    return _name_index;
  }
  
  short MethodInfo::get_descriptor_index() const
  {
    return _descriptor_index;
  }

  
}
