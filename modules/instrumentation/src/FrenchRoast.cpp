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

#include "FrenchRoast.h"
#include "ConstantPool.h"
#include "Util.h"
#include "StackMapFrame.h"
#include "OpCode.h"
#include "OpCodeConst.h"
#include "Instruction.h"
#include "fr_signals.h"

namespace frenchroast {

  invalid_method_descriptor::invalid_method_descriptor(const std::string& msg) : std::invalid_argument(msg)
  {
  }
  
  int MagicComponent::size_in_bytes() const
  {
    return sizeof(_number);
  }
  
  void MagicComponent::reset()
  {
    memset(_number, 0, sizeof(_number));
  }

  void MagicComponent::load_from_buffer(const BYTE* buf)
  {
    memcpy(_number, buf,sizeof(_number));
  }

  void MagicComponent::load_to_buffer(BYTE* buf)
  {
    memcpy(buf, _number, sizeof(_number));
  }

  int MagicComponent::get_number()
  {
    return to_int(_number,sizeof(_number));
  }

  void MagicComponent::display(std::ostream& out)
  {
    std::ios_base::fmtflags flags = out.flags();
    out << std::hex << std::uppercase << "magic: " << to_int(_number,sizeof(_number)) << std::endl;
    out.flags(flags);
  }
    
  int  IDComponent::size_in_bytes() const
  {
    return sizeof(_version);
  }

  void IDComponent::reset()
  {
    memset(_version, 0, sizeof(_version));
  }

  void IDComponent::load_from_buffer(const BYTE* buf)
  {
    memcpy(_version, buf,sizeof(_version));
  }

  void IDComponent::load_to_buffer(BYTE* buf)
  {
    memcpy(buf, _version, sizeof(_version));
  }

  int  IDComponent::get()
  {
    return to_int(_version,sizeof(_version));
  }

  void IDComponent::display(std::ostream& out)
  {
    out << to_int(_version,sizeof(_version)) << std::endl;
  }


  int InterfacesComponent::size_in_bytes() const
  {
    return static_cast<int>(sizeof(_count) + _items.size() * 2) ;
  }

  int InterfacesComponent::get_count() const
  {
    return to_int(_count,2);
  } 

  void InterfacesComponent::reset()
  {
    memset(_count, 0, sizeof(_count)); _items.clear();
  }

  void InterfacesComponent::load_from_buffer(const BYTE* buf)
  {
    memcpy(_count, buf,sizeof(_count));
    int lcount = to_int(_count, sizeof(_count));
    for (int idx = 0; idx < lcount; idx++) {
      short item;
      memcpy(&item, buf + sizeof(_count) + idx*2, 2);
      _items.push_back(item);
    }
  }

  void InterfacesComponent::load_to_buffer(BYTE* buf)
  {
    memcpy(buf, _count, sizeof(_count));
    int idx = 1;
    for (auto& x : _items) {
      memcpy(buf + idx * 2, &x, 2);
      ++idx;
    }
  }
    
  void InterfacesComponent::display(std::ostream& out)
  {
    int idx  = to_int(_count,sizeof(_count));
    out << "interfaces: " << to_int(_count, sizeof(_count)) << std::endl;
    short hh;
    for (auto& x : _items) {
      out << "  index " << bits_to_int<short>(hh,(BYTE*)&x) << std::endl;
    }
  }

  template
  <typename T,typename H>
  int InfoComponent<T,H>::size_in_bytes() const
  {
    int rv = sizeof(_count);
    for (auto& x : _items) {
      rv += x.size_in_bytes();
    }
    return rv;
  }
    
  template
  <typename T,typename H>
  void InfoComponent<T,H>::reset()
  {
    memset(_count, 0, sizeof(_count));
    _items.clear();
    _name_index_map.clear();

  }

  template
  <typename T,typename H>
  std::vector<std::string> InfoComponent<T,H>::get_items_names()
  {
    std::vector<std::string> rv;
    for(auto& x : _name_index_map) {
      rv.push_back(x.first);
    }
    return rv;
  }
  
  template
  <typename T,typename H>
  void InfoComponent<T,H>::load_from_buffer(const BYTE* buf)
  {
    memcpy(_count, buf, sizeof(_count));
    int bytes = sizeof(_count);
    for (int idx=0; idx < to_int(_count,sizeof(_count));idx++) {
      H fld{_resolver};
      bytes += fld.load_from_buffer(buf + bytes);
      _name_index_map[fld.get_name(_resolver)]=idx;
      _items.push_back(std::move(fld));
    }
  }
    
  template
  <typename T,typename H>
  void InfoComponent<T,H>::load_to_buffer(BYTE* buf)
  {
    memcpy(buf, _count, sizeof(_count));
    int bytes = sizeof(_count);
    for (auto& x : _items) {
      bytes += x.load_to_buffer(buf + bytes);
    }
  }

  template
  <typename T,typename H>
  void InfoComponent<T,H>::display(std::ostream& out)
  {
    out << "\n------------- " << _desc << " ( " << to_int(_count,sizeof(_count)) << " )--------------\n"<< std::endl;
    for(auto& x : _items) {
      x.display(out,_resolver);
    }
  }

  template
  <typename T,typename H>
  InfoComponent<T,H>::InfoComponent(T& ptr, const std::string& name) : _desc(name), _resolver(ptr) {}

  template
  <typename T,typename H>
  bool InfoComponent<T,H>::has_item(const std::string& name) {
    return _name_index_map.count(name) == 1;
  }

  template
  <typename T,typename H>
  H& InfoComponent<T,H>::get_item(const std::string& name)
  {
    // add error check
    return _items[_name_index_map[name]];
  }

  template
  <typename T,typename H>
  void InfoComponent<T,H>::add_item(H& item)
  {
    incr(_count,sizeof(_count));
    _name_index_map[item.get_name(_resolver)]=to_int(_count,sizeof(_count));
    _items.push_back(std::move(item));
  }

  template
  <typename T,typename H>
  int InfoComponent<T,H>::get_count() const
  {
    return to_int(_count,2);
  }

    
    
  void FrenchRoast::reset()
  {
    for (auto& x : _components) {
      x->reset();
    }
  }
    
  FrenchRoast::FrenchRoast(frenchroast::OpCode opcodes) : _opCodes(opcodes), _methodsComponent(_constPool)
  {    
  }



  std::vector<std::string> FrenchRoast::get_method_descriptors()
  {
    return _methodsComponent.get_method_descriptors();
  }
  
  std::string FrenchRoast::resolve_const(int idx)
  {
    return _constPool.resolve_name(idx);
  }
    
  int FrenchRoast::size_in_bytes()
  {
    int rv = 0;
    for(auto& x : _components) {
      rv += x->size_in_bytes();
    }
    return rv;
  }

  void FrenchRoast::load_from_buffer(const BYTE* kbuf)
  {
    reset();
    int idx=0;
    _magic.load_from_buffer(kbuf);
    _minorVersion.load_from_buffer(kbuf + (idx+=_magic.size_in_bytes()));
    _majorVersion.load_from_buffer(kbuf + (idx+=_minorVersion.size_in_bytes()));

    _constPool.load_from_buffer(kbuf +    (idx+=_majorVersion.size_in_bytes()));
    _accessFlags.load_from_buffer(kbuf +  (idx+=_constPool.size_in_bytes()));
    _thisClass.load_from_buffer(kbuf +    (idx+=_accessFlags.size_in_bytes()));

    _superClass.load_from_buffer(kbuf +   (idx+=_thisClass.size_in_bytes()));
    _interfaces.load_from_buffer(kbuf +   (idx+=_superClass.size_in_bytes()));

    _fieldsComponent.load_from_buffer(kbuf +   (idx+=_interfaces.size_in_bytes()));

    _methodsComponent.load_from_buffer(kbuf +   (idx+=_fieldsComponent.size_in_bytes()));
      idx+=_methodsComponent.size_in_bytes();
    _attributeComponent.load_from_buffer(kbuf +   idx);
    idx+=_attributeComponent.size_in_bytes();
  }

  void FrenchRoast::load_to_buffer(BYTE* kbuf)
  {
    int idx=0;
    _magic.load_to_buffer(kbuf);
    _minorVersion.load_to_buffer(kbuf + (idx+=_magic.size_in_bytes()));
    _majorVersion.load_to_buffer(kbuf + (idx+=_minorVersion.size_in_bytes()));
    _constPool.load_to_buffer(kbuf +    (idx+=_majorVersion.size_in_bytes()));
    _accessFlags.load_to_buffer(kbuf +  (idx+=_constPool.size_in_bytes()));
    _thisClass.load_to_buffer(kbuf +    (idx+=_accessFlags.size_in_bytes()));
    _superClass.load_to_buffer(kbuf +   (idx+=_thisClass.size_in_bytes()));
    _interfaces.load_to_buffer(kbuf +   (idx+=_superClass.size_in_bytes()));
    _fieldsComponent.load_to_buffer(kbuf +   (idx+=_interfaces.size_in_bytes()));
    _methodsComponent.load_to_buffer(kbuf +   (idx+=_fieldsComponent.size_in_bytes()));
    _attributeComponent.load_to_buffer(kbuf +   (idx+=_methodsComponent.size_in_bytes()));
  }

  void FrenchRoast::display(std::ostream& out)
  {
    _magic.display(std::cout);
    _minorVersion.display(std::cout);
    _majorVersion.display(std::cout);
    _accessFlags.display(std::cout);
    _thisClass.display(std::cout);
    _superClass.display(std::cout);
    _constPool.display(std::cout);
    _interfaces.display(std::cout);
    _fieldsComponent.display(std::cout);
    _methodsComponent.display(std::cout);
    _attributeComponent.display(std::cout);
  }

  short FrenchRoast::add_constant_integer_value(int value)
  {
    return (_constPool + new IntegerHolder(value));
  }

  void FrenchRoast::modify_constant(const std::string& fieldname, short valueIndex)
  {
    Info<FrenchRoast>& info = _fieldsComponent.get_item(fieldname);
    ConstantValueAttribute* ptr = info.get_constant_value_attribute();
    write_big_e_bytes(ptr->_valueIndex,&valueIndex);
    Info<FrenchRoast>& info2 = _fieldsComponent.get_item(fieldname);
    ConstantValueAttribute* ptr2 = info.get_constant_value_attribute();
  }

  void FrenchRoast::add_name_to_pool(const std::string& vname)
  {
    _constPool.add_name(vname);
  }
    
  void FrenchRoast::add_native_method(const std::string& vname, const std::string& vtype)
  {
    short flags = AccessFlagsComponent::ACC_NATIVE | AccessFlagsComponent::ACC_PUBLIC | AccessFlagsComponent::ACC_STATIC;
    MethodInfo method{flags, static_cast<short>(_constPool.add_name(vname)), static_cast<short>(_constPool.add_name(vtype))};
    _methodsComponent.add_method(std::move(method));
  }

  void FrenchRoast::add_static_field(const std::string& vname, const std::string& vtype)
  {
    short descriptorIndex = _constPool.add_name(vtype);
    short nameIndex       = _constPool.add_name(vname);
    Info<FrenchRoast> field{*this};
    short flags = AccessFlagsComponent::ACC_PUBLIC | AccessFlagsComponent::ACC_STATIC;
    field.set_flags(flags);
    field.set_name_index(nameIndex);
    field.set_descriptor_index(descriptorIndex);
    _fieldsComponent.add_item(field);
  }


  void FrenchRoast::add_heap_monitor(MethodInfo& meth, int hook_id)
  {
    std::vector<Instruction> instructions;
    instructions.push_back(Instruction(_opCodes[opcode::aload_0]));
    instructions.push_back(Instruction(_opCodes[opcode::invokestatic],  hook_id));
    meth.add_instructions(MethodInfo::FIRST_ADDRESS_AFTER_INIT,instructions, true);
    if (meth.get_max_stack() == 0) {
      meth.set_max_stack(1);
      }
  }
  
  void FrenchRoast::update_method(MethodInfo& meth, std::bitset<4> flags, const std::string& callTo)
  {
    int hook_id =   _constPool.add_method_ref_index(callTo);
    if ((flags & frenchroast::signal::Signals::MONITOR_HEAP) == frenchroast::signal::Signals::MONITOR_HEAP) {
      add_heap_monitor(meth,hook_id);
      return;
    }

    
    if ((flags & frenchroast::signal::Signals::METHOD_ENTER) == frenchroast::signal::Signals::METHOD_ENTER) {
      std::vector<Instruction> instructions;
      instructions.push_back(Instruction(_opCodes[opcode::aload_0]));
      instructions.push_back(Instruction(_opCodes[opcode::invokestatic],  hook_id));
      meth.add_instructions(0,instructions, true);
      if (meth.get_max_stack() == 0) {
        meth.set_max_stack(1);
      }
    }
    
    if ((flags & frenchroast::signal::Signals::METHOD_EXIT) == frenchroast::signal::Signals::METHOD_EXIT) {
      std::vector<Instruction> instructions;
      instructions.push_back(Instruction(_opCodes[opcode::aload_0]));
      instructions.push_back(Instruction(_opCodes[opcode::invokestatic],  hook_id));
      bool adjust = false;
      for (auto& x : meth.get_return_addresses()) {
         meth.add_instructions(x,instructions, adjust);
         adjust = true;
      }
      if (meth.get_max_stack() == 0) {
        meth.set_max_stack(1);
      }

    }
    if ((flags & frenchroast::signal::Signals::METHOD_TIMER) == frenchroast::signal::Signals::METHOD_TIMER) {
      int enter_id = _constPool.add_string("enter");
      int exit_id = _constPool.add_string("exit");
      _constPool.add_name("java/lang/System");
      _constPool.add_name("java/lang/Object");
      _constPool.add_name("java/lang/Thread");
      _constPool.add_name("java/lang/String");
      
      _constPool.add_name("()Ljava/lang/Thread");
      _constPool.add_name("()Ljava/lang/String");
            
      int currenttimeMillis_id =   _constPool.add_method_ref_index("java/lang/System.currentTimeMillis:()J");
      int currentThread_id =       _constPool.add_method_ref_index("java/lang/Thread.currentThread:()Ljava/lang/Thread;");
      int getname_id =             _constPool.add_method_ref_index("java/lang/Thread.getName:()Ljava/lang/String;");
      
      std::vector<Instruction> instructions;
      instructions.push_back(Instruction(_opCodes[opcode::invokestatic],  currenttimeMillis_id));
      instructions.push_back(Instruction(_opCodes[opcode::ldc],           enter_id));
      instructions.push_back(Instruction(_opCodes[opcode::invokestatic],  currentThread_id));
      instructions.push_back(Instruction(_opCodes[opcode::invokevirtual], getname_id));
      instructions.push_back(Instruction(_opCodes[opcode::invokestatic],  hook_id));
      meth.add_instructions(0,instructions, true);      
      Instruction ninst{_opCodes[opcode::ldc], static_cast<short>(exit_id)};
      instructions[1] = ninst;
      //@@@      instructions[1] = std::move(Instruction(_opCodes[opcode::ldc], exit_id));
      bool adjust = false;
      for (auto& x : meth.get_return_addresses()) {
         meth.add_instructions(x,instructions, adjust);
         adjust = true;
      }
      meth.set_max_stack( meth.get_max_stack() + 4);
    }
  }

  void FrenchRoast::add_method_call(const std::string& callFrom, const std::string& callTo, std::bitset<4> flags)
  {
    if (!_methodsComponent.has_method(callFrom)) {
      throw invalid_method_descriptor{callFrom};
    }
    ConstantPoolComponent::validate_method_name(callTo); 
    MethodInfo method = _methodsComponent.get_method(callFrom);
    update_method(method, flags, callTo);
    _methodsComponent.add_method(std::move(method));
  }
    
}





