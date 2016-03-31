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
#include "Method.h"

namespace frenchroast {

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
    return sizeof(_count) + _items.size() * 2 ;
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

    
  const std::bitset<4> FrenchRoast::METHOD_ENTER{"0001"};
  const std::bitset<4> FrenchRoast::METHOD_EXIT{"0010"};

    
  void FrenchRoast::reset()
  {
    for (auto& x : _components) {
      x->reset();
    }
  }
    
  FrenchRoast::FrenchRoast() : _opcodesLoaded(false)
  {
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
    _attributeComponent.load_from_buffer(kbuf +   (idx+=_methodsComponent.size_in_bytes()));
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
    short descriptorIndex = _constPool.add_name(vtype);
    short nameIndex       = _constPool.add_name(vname);
    Info<FrenchRoast> method{*this};
    short flags = AccessFlagsComponent::ACC_NATIVE | AccessFlagsComponent::ACC_PUBLIC | AccessFlagsComponent::ACC_STATIC;
    method.set_flags(flags);
    method.set_name_index(nameIndex);
    method.set_descriptor_index(descriptorIndex);
    _methodsComponent.add_item(method);
  }


  void FrenchRoast::load_op_codes(const std::string& fileName)
  {
    if (_opcodesLoaded)
      {
	return;
      }
    _opCodes.load(fileName);
    _opcodesLoaded = true;
  }
    
  void FrenchRoast::add_method_call(const std::string& callFrom, const std::string& callTo, std::bitset<4> flags)
  {
    if (!_methodsComponent.has_item(callFrom)) {
      std::cout << "ERROR, does not Exist: " << callFrom << std::endl; 
    }
    ConstantPoolComponent::validate_method_name(callTo); 
    Method method;
    // * LOAD
    BYTE* methodBuffer = _methodsComponent.get_item(callFrom).get_attribute("Code")._info;
    method.load_from_buffer(methodBuffer);
                        
    if (method.get_max_stack() == 0) {
      method.set_max_stack(1);
    }

    int exception_table_length = to_int(methodBuffer + 8 + to_int(methodBuffer +4,4),2);
    int attribStart = 8 + to_int(methodBuffer +4,4)  + exception_table_length + 2;

    InfoComponent<FrenchRoast, Attribute<FrenchRoast>> methodAttributes{*this,"Attributes"};
    methodAttributes.load_from_buffer(methodBuffer + attribStart);


    
    int origAttributeLength = _methodsComponent.get_item(callFrom).get_attribute("Code").get_length();
    int origCodeLength      = method.size_in_bytes();
    int origNonCodeLength   = origAttributeLength - origCodeLength;
    std::vector<Instruction> instructions;
    int idxid =   _constPool.add_method_ref_index(callTo);
    instructions.push_back(Instruction(_opCodes[opcode::invokestatic],  idxid));
    int startAddress = 0;

    // ---------------> handle flags correctly
    // method.add_instructions(startAddress,instructions,true);
    //method.add_instructions(17,instructions,true);
    method.add_instructions(30,instructions,true);
    //method.add_instructions(50,instructions,false);
    
    if (methodAttributes.has_item("StackMapTable")) {
      BYTE* ptr = methodAttributes.get_item("StackMapTable")._info;
      short frameCount = to_int(ptr,2);
      std::vector<StackMapFrame*> frames = load_frames_from_buffer(frameCount, ptr + 2);
      int origframebytes = 0;
      for(auto& x : frames) {
	 origframebytes += x->size_in_bytes();
      }

      origNonCodeLength -= origframebytes;
      method.adjust_frames(frames);
      delete[] methodAttributes.get_item("StackMapTable")._info;
      int framebytes = 0;
      for(auto& x : frames) {
	framebytes += x->size_in_bytes();
      }
      origNonCodeLength += framebytes;
      methodAttributes.get_item("StackMapTable")._info = new BYTE[framebytes +2];
      ptr = methodAttributes.get_item("StackMapTable")._info;
      write_bytes(ptr,frameCount,2);
      load_frames_to_buffer(frames,ptr + 2);
    }
    
    _methodsComponent.get_item(callFrom).get_attribute("Code").set_length(method.size_in_bytes() + origNonCodeLength);

    BYTE* newbuf = new BYTE[method.size_in_bytes() + origNonCodeLength ];
    method.load_to_buffer(newbuf);
    memcpy(newbuf + method.size_in_bytes(), _methodsComponent.get_item(callFrom).get_attribute("Code")._info + origCodeLength, origNonCodeLength);
    
    exception_table_length = to_int(newbuf + 8 + to_int(newbuf +4,4),2);
    attribStart = 8 + to_int(newbuf +4,4)  + exception_table_length + 2;

    
    methodAttributes.load_to_buffer(newbuf + attribStart);
    delete[] _methodsComponent.get_item(callFrom).get_attribute("Code")._info;
   _methodsComponent.get_item(callFrom).get_attribute("Code")._info = newbuf;

  
}
    
}





