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

#ifndef FROAST_H
#define FROAST_H


#include <iostream>
#include <vector>
#include <bitset>
#include <unordered_map>
#include "ClassFileComponent.h"
#include "ConstantPool.h"
#include "Info.h"
#include "AccessFlags.h"
#include "OpCode.h"
#include "Method.h"


namespace frenchroast {

  class MagicComponent : public ClassFileComponent {
    BYTE _number[4];
  public:
    int  size_in_bytes() const;
    void reset();
    void load_from_buffer(const BYTE* buf);
    void load_to_buffer(BYTE* buf);
    void display(std::ostream& out);
    int get_number();
  };

  class IDComponent : public ClassFileComponent {
    BYTE _version[2];
  public:
    int  size_in_bytes() const;
    void reset();
    void load_from_buffer(const BYTE* buf);
    void load_to_buffer(BYTE* buf);   
    void display(std::ostream& out);
    int get();
  };

  
  class InterfacesComponent : public ClassFileComponent {
    BYTE               _count[2];
    std::vector<short> _items;
  public:
    int  size_in_bytes() const;
    int  get_count() const;
    void reset();
    void load_from_buffer(const BYTE* buf);
    void load_to_buffer(BYTE* buf);
    void display(std::ostream& out);
  };

  template  
  <typename T,typename H>
  class InfoComponent : public ClassFileComponent   {
    BYTE           _count[2];
    std::vector<H> _items;
    std::unordered_map<std::string,int> _name_index_map;
    T&             _resolver;
    std::string    _desc;
  public:
    int  size_in_bytes() const;
    void reset();
    void load_from_buffer(const  BYTE* buf);
    void load_to_buffer(BYTE* buf);
    void display(std::ostream& out);
    InfoComponent( T& resolver,const std::string& desc);
    H& get_item(const std::string&);
    bool has_item(const std::string&);
    void add_item(H& item);
    int get_count() const;
  };
   
  class FrenchRoast {
    OpCode                 _opCodes;
    MagicComponent         _magic;
    IDComponent            _minorVersion;
    IDComponent            _majorVersion;
    ConstantPoolComponent  _constPool;
    AccessFlagsComponent   _accessFlags;
    IDComponent            _thisClass;
    IDComponent            _superClass;
    InterfacesComponent    _interfaces;
    InfoComponent<FrenchRoast, Info<FrenchRoast>>  _fieldsComponent{*this, "Fields"};
    InfoComponent<FrenchRoast, Info<FrenchRoast>> _methodsComponent{*this,"Methods"};
    InfoComponent<FrenchRoast, Attribute<FrenchRoast>> _attributeComponent{*this,"Attributes"};
    
    std::vector<ClassFileComponent*> _components{&_magic,
                                               &_minorVersion,
                                               &_majorVersion,
                                               &_constPool,
                                               &_accessFlags,
                                               &_thisClass,
                                               &_superClass,
                                               &_interfaces,
                                               &_fieldsComponent,
                                               &_methodsComponent,
                                               &_attributeComponent
                                              };

  
    void load_magic(unsigned char* magic, unsigned char* buf);
    void reset();
    bool _opcodesLoaded;
    void update_method(Method& meth, std::bitset<4> flags, int constPoolId);
  public:
    void load_from_buffer(const BYTE* buf);
    void load_to_buffer(BYTE* buf);
    void display(std::ostream& out);
    int  size_in_bytes();
    std::string resolve_const(int idx);
    void add_name_to_pool(const std::string& vname);
    void add_native_method(const std::string& vname, const std::string& vtype);
    void add_method_call(const std::string& callFrom, const std::string& callTo, std::bitset<4> flags);
    short add_constant_integer_value(int value);
    void modify_constant(const std::string& fieldname, short descriptorIndex);
    FrenchRoast();
    void load_op_codes(const std::string& fileName);
    
    static const std::bitset<4> METHOD_ENTER;
    static const std::bitset<4> METHOD_EXIT;
    static const std::bitset<4> METHOD_TIMER;
  };

}
#endif
