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
#include "catch.hpp"
#include "MethodInfo.h"
#include "AccessFlags.h"
#include "OpCodeConst.h"
#include "FrenchRoast.h"

using namespace frenchroast;

BYTE* build_simple_method(int& size, std::unordered_map<std::string, short>& ids);


BYTE* build_native_method(int& size, std::unordered_map<std::string, short>& ids)
{
  short accessFlags =  AccessFlagsComponent::ACC_NATIVE | AccessFlagsComponent::ACC_PUBLIC | AccessFlagsComponent::ACC_STATIC;
  short nameIndex   = ids["SomeFunc"];
  short descriptorIndex   = ids["()V"];
  short attributesCount = 0;

  size = 8;

  BYTE* buf = new BYTE[size];
  BYTE* ptr = buf;
  wbytes(ptr , &accessFlags);
  wbytes(ptr , &nameIndex);
  wbytes(ptr , &descriptorIndex);
  wbytes(ptr , &attributesCount);

  return buf;
}


TEST_CASE ( "method) = opcode:: [simple] save to buffer")
{
  using namespace frenchroast;
  std::unordered_map<int, std::string> names;
  names[1] = "SomeFunc";
  names[2] = "()V";
  names[3] = "Code";
  names[4] = "StackMapTable";
  

  std::unordered_map<std::string, int> ids;
  for(auto& item : names) {
    ids[item.second] = item.first;
  }
  std::unordered_map<std::string, short> sids;
  for(auto& item : names) {
    sids[item.second] = item.first;
  }

  OpCode::load_from_file( std::getenv("OPCODE_FILE"));
  int size;
  BYTE* buf = build_simple_method(size,sids);

  MethodInfo meth;
  meth.load_from_buffer(buf,names);
  REQUIRE(meth.size_in_bytes() == size);
  REQUIRE(meth[7].address() == 11);
  REQUIRE((meth[7].opcode()  == opcode::xreturn)  );
  REQUIRE(meth[3].offset() == 8);
  REQUIRE(meth[3].address() == 3);


  BYTE* newbuf = new BYTE[size];
  memset(newbuf,0,size);
  meth.load_to_buffer(newbuf,ids);

  REQUIRE(memcmp(buf,newbuf,size) == 0);

  MethodInfo newmeth;
  newmeth.load_from_buffer(newbuf,names);
  REQUIRE(newmeth.size_in_bytes() == meth.size_in_bytes());
  REQUIRE(newmeth.frames().size()  == 1);

          
  delete[] buf;
  delete[] newbuf;

}


TEST_CASE ( "method) = opcode:: [add_instructions] save to buffer")
{
  using namespace frenchroast;
  std::unordered_map<int, std::string> names;
  names[1] = "SomeFunc";
  names[2] = "()V";
  names[3] = "Code";
  names[4] = "StackMapTable";
  

  std::unordered_map<std::string, int> ids;
  for(auto& item : names) {
    ids[item.second] = item.first;
  }

  std::unordered_map<std::string, short> sids;
  for(auto& item : names) {
    sids[item.second] = item.first;
  }

  
  OpCode::load_from_file( std::getenv("OPCODE_FILE"));
  int size;
  BYTE* buf = build_simple_method(size,sids);
  MethodInfo meth;
  meth.load_from_buffer(buf,names);


  std::vector<Instruction> list;
  OpCode ops;
  list.push_back(Instruction{ops[opcode::invokestatic], 9});

  
  REQUIRE(meth[7].address() == 11);
  REQUIRE((meth[7].opcode()  == opcode::xreturn ) );
  REQUIRE(meth[3].offset() == 8);
  REQUIRE(meth.size_in_bytes() == size );

  meth.add_instructions(0,list,true);
  REQUIRE(meth.size_in_bytes() == size + 3);
  REQUIRE(meth[8].address() == 14);
  REQUIRE((opcode::xreturn == meth[8].opcode()) );
  REQUIRE(meth[4].address() == 6);
  REQUIRE(meth[4].offset() == 8);

  

  BYTE* newbuf = new BYTE[size + 3];
  meth.load_to_buffer(newbuf,ids);

  MethodInfo newmeth;
  newmeth.load_from_buffer(newbuf,names);
  REQUIRE(newmeth.size_in_bytes() == meth.size_in_bytes());
  REQUIRE(newmeth.frames().size()  == 1);

          
  delete[] buf;
  delete[] newbuf;

}


TEST_CASE ( "method) = opcode:: [native method] load/save to buffer")
{
  using namespace frenchroast;
  std::unordered_map<int, std::string> names;
  names[1] = "SomeFunc";
  names[2] = "()V";

  std::unordered_map<std::string, int> ids;
  for(auto& item : names) {
    ids[item.second] = item.first;
  }
  std::unordered_map<std::string, short> sids;
  for(auto& item : names) {
    sids[item.second] = item.first;
  }

  OpCode::load_from_file( std::getenv("OPCODE_FILE"));
  int size;
  BYTE* buf = build_native_method(size,sids);

  MethodInfo meth;

  meth.load_from_buffer(buf,names);

  REQUIRE(meth.size_in_bytes() == 8 );

  BYTE* newbuf = new BYTE[size];


  
  meth.load_to_buffer(newbuf,ids);

  MethodInfo newmeth;
  newmeth.load_from_buffer(newbuf,names);
  REQUIRE(newmeth.size_in_bytes() == meth.size_in_bytes());
          
  delete[] buf;
  delete[] newbuf;
  
}

TEST_CASE ( "method) = opcode:: [native method] build/save to buffer")
{
  using namespace frenchroast;
  std::unordered_map<int, std::string> names;
  names[1] = "SomeFunc";
  names[2] = "()V";

  std::unordered_map<std::string, short> sids;
  for(auto& item : names) {
    sids[item.second] = item.first;
  }

  std::unordered_map<std::string, int> ids;
  for(auto& item : names) {
    ids[item.second] = item.first;
  }

  OpCode::load_from_file( std::getenv("OPCODE_FILE"));
  int size;
  BYTE* buf = build_native_method(size,sids);

  MethodInfo meth;

  meth.set_access_flags(AccessFlagsComponent::ACC_NATIVE | AccessFlagsComponent::ACC_PUBLIC | AccessFlagsComponent::ACC_STATIC);
  meth.set_name_index(ids["SomeFunc"]);
  meth.set_descriptor_index(ids["()V"]);
  
    
  REQUIRE(meth.size_in_bytes() == 8 );

  BYTE* newbuf = new BYTE[size];
  meth.load_to_buffer(newbuf,ids);

  MethodInfo newmeth;
  newmeth.load_from_buffer(newbuf,names);
  REQUIRE(newmeth.size_in_bytes() == meth.size_in_bytes());
  REQUIRE(memcmp(newbuf,buf,size) == 0);
          
  delete[] buf;
  delete[] newbuf;
  
}






  
