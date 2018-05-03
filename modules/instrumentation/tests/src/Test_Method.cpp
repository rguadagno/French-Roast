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
#include <unordered_map>
#include "catch.hpp"
#include "ClassFileComponent.h"
#include "Util.h"
#include "Instruction.h"
#include "OpCodeConst.h"
#include "MethodInfo.h"
#include "StackMapFrame.h"
#include "FrameConst.h"
#include "AccessFlags.h"


using namespace frenchroast;


BYTE* build_zero_exceptions(int& size)
{
  size = 2;
  BYTE* buf = new BYTE[2];
  short exceptionCount = 0;
  write_big_e_bytes(buf, &exceptionCount);
  return buf;
}



BYTE* build_stack_maps_for_simple(int& size,  std::unordered_map<std::string,short>& ids)
{
  short name_index = ids["StackMapTable"];
  int attrib_length;
  short count = 1;
  BYTE  frame    = 11;


  size = sizeof(name_index) + sizeof(attrib_length) + sizeof(count) + sizeof(frame);
  attrib_length = sizeof(count) + sizeof(frame);
  BYTE* buf = new BYTE[size];
  BYTE* ptr = buf;
  wbytes(ptr,&name_index);
  wbytes(ptr,&attrib_length);
  wbytes(ptr,&count);
  wbytes(ptr,&frame);
  
  return buf;
}

BYTE* build_simple_method(int& size, std::unordered_map<std::string, short>& ids)
{
/*
   ----------- method ------------------

    public void donothing(int i) {
       if (i < 10) {
     	   something(i);
	}
    }

   --------------------------------------   
 
*/

  short accessFlags = AccessFlagsComponent::ACC_PUBLIC;
  short nameIndex   = ids["SomeFunc"];
  short descriptorIndex   = ids["()V"];
  short attributesCount = 1;

  short attrib_name_index = ids["Code"];
  short maxStack = 2;
  short maxLocals = 2;
  int codeLength = 12;
  short exceptionTableLength = 0;
  short codeAttributeCount =1 ;

  int stack_map_size;
  BYTE* sm_buf = build_stack_maps_for_simple(stack_map_size,ids);
  int   attribute_length  = sizeof(maxStack) + sizeof(maxLocals) + sizeof(codeLength) +
    codeLength + sizeof(exceptionTableLength) + sizeof(codeAttributeCount) + stack_map_size;


  size = sizeof(accessFlags) +
         sizeof(nameIndex) +
         sizeof(descriptorIndex) +
         sizeof(attributesCount) +
         6 + 
         attribute_length
       
    ;

  BYTE* buf = new BYTE[size];
  BYTE* ptr = buf;
  wbytes(ptr , &accessFlags);
  wbytes(ptr , &nameIndex);
  wbytes(ptr , &descriptorIndex);
  wbytes(ptr , &attributesCount);
  
  wbytes(ptr , &attrib_name_index);
  wbytes(ptr , &attribute_length);
  
  wbytes(ptr , &maxStack);
  wbytes(ptr , &maxLocals);
  wbytes(ptr , &codeLength);

  *(ptr + 0) = opcode::iload_1;
  *(ptr + 1) = opcode::bipush; *(ptr + 2) = 10;
  *(ptr + 3) = opcode::if_icmpge; write_bytes(ptr + 4, 8, 2);
  *(ptr + 6) = opcode::aload_0;
  *(ptr + 7) = opcode::iload_1;
  *(ptr + 8) = opcode::invokevirtual; write_bytes(ptr + 9, 7, 2);
  *(ptr + 11) = opcode::xreturn;
  ptr += 12;
  int zero_excep_size;
  BYTE* excep_buf = build_zero_exceptions(zero_excep_size);
  memcpy(ptr,excep_buf, zero_excep_size);
  ptr += zero_excep_size;
  short codeAttribLen = 1;
  wbytes(ptr,&codeAttribLen);
  memcpy(ptr,sm_buf,stack_map_size);
  return buf;
}



BYTE* build_stack_maps_for_lookupswitch(int& size, std::unordered_map<std::string, short>& ids)
{
  short name_index = ids["StackMapTable"];
  int attrib_length;
  short count = 3;

  size = sizeof(name_index) + sizeof(attrib_length) + sizeof(count) + count;
  attrib_length = sizeof(count) + 3;

  BYTE* buf = new BYTE[size];
  BYTE* ptr = buf;
  wbytes(ptr,&name_index);
  wbytes(ptr,&attrib_length);
  wbytes(ptr,&count);

  
  *(ptr + 0) = 28;
  *(ptr + 1) = 10;
  *(ptr + 2) = 7;
  return buf;
}



BYTE* build_lookupswitch_method(int& size, std::unordered_map<std::string, short>& ids)
{
/*
   ----------- method ------------------

public void something(int x) {
        switch(x) {
        case 10:   
            System.out.println("okok");     
            break;
    
        case 20:
            System.out.println("xxx");
            break;
        }
    }

*/

  short accessFlags = AccessFlagsComponent::ACC_PUBLIC;
  short nameIndex   = ids["SomeFunc"];
  short descriptorIndex   = ids["()V"];
  short attributesCount = 1;

  short attrib_name_index = ids["Code"];
  short maxStack = 2;
  short maxLocals = 2;
  int codeLength = 48;
  short exceptionTableLength = 0;
  short codeAttributeCount =1 ;

  
  int stack_map_size;
  BYTE* sm_buf = build_stack_maps_for_lookupswitch(stack_map_size,ids);



  int   attribute_length  = sizeof(maxStack) + sizeof(maxLocals) + sizeof(codeLength) +
    codeLength + sizeof(exceptionTableLength) + sizeof(codeAttributeCount) + stack_map_size;


  size = sizeof(accessFlags) +
         sizeof(nameIndex) +
         sizeof(descriptorIndex) +
         sizeof(attributesCount) +
         6 + 
         attribute_length;

  BYTE* buf = new BYTE[size];
  BYTE* ptr = buf;
  wbytes(ptr , &accessFlags);
  wbytes(ptr , &nameIndex);
  wbytes(ptr , &descriptorIndex);
  wbytes(ptr , &attributesCount);
  
  wbytes(ptr , &attrib_name_index);
  wbytes(ptr , &attribute_length);
  
  wbytes(ptr , &maxStack);
  wbytes(ptr , &maxLocals);
  wbytes(ptr , &codeLength);

  BYTE* code = ptr;
  *(code + 0) = opcode::iload_1;
  *(code + 1) = opcode::lookupswitch;
  int pad = Instruction::calc_pad(1);
  write_bytes(code + 2 + pad, 46, 4);
  const int numberOfPairs = 2;
  const short DOES_NOT_MATTER = 1; // since code is not to be run just make it clear
  write_bytes(code + 2 + pad + 4, numberOfPairs, 4);
  write_bytes(code + 2 + pad + 4 + 4, DOES_NOT_MATTER, 4);
  write_bytes(code + 2 + pad + 4 + 8, 27, 4);
  write_bytes(code + 2 + pad + 4 + 12, DOES_NOT_MATTER, 4);
  write_bytes(code + 2 + pad + 4 + 16, 38, 4);
 
  *(code + 28) = opcode::getstatic;     write_bytes(code + 29, DOES_NOT_MATTER, 2);
  *(code + 31) = opcode::ldc;           *(code + 32) = DOES_NOT_MATTER;
  *(code + 33) = opcode::invokevirtual; write_bytes(code + 34, DOES_NOT_MATTER, 2);
  *(code + 36) = opcode::xgoto;         write_bytes(code + 37, 11, 2);
  *(code + 39) = opcode::getstatic;     write_bytes(code + 40, DOES_NOT_MATTER, 2);
  *(code + 42) = opcode::ldc;           *(code + 43) = DOES_NOT_MATTER;
  *(code + 44) = opcode::invokevirtual; write_bytes(code + 45, DOES_NOT_MATTER, 2);
  *(code + 47) = opcode::xreturn;

  //-----------------------------
  ptr += 48;
  int zero_excep_size;
  BYTE* excep_buf = build_zero_exceptions(zero_excep_size);
  memcpy(ptr,excep_buf, zero_excep_size);
  ptr += zero_excep_size;
  short codeAttribLen = 1;
  wbytes(ptr,&codeAttribLen);
  memcpy(ptr,sm_buf,stack_map_size);

  
  return buf;
}


BYTE* build_lookupswitch_return_method(int& size, std::unordered_map<std::string, short>& ids)
{
  

  //   ----------- method ------------------

  //public void something(int x) {
  //    switch(x) {
  //    case 10:   
  //        System.out.println("okok");     
  //        return;
  //
  //    case 20
  //        System.out.println("xxx");
  //        break;
  //    }
  //    System.out.println("NOTHING");
  //}

  short accessFlags = AccessFlagsComponent::ACC_PUBLIC;
  short nameIndex   = ids["SomeFunc"];
  short descriptorIndex   = ids["()V"];
  short attributesCount = 1;

  short attrib_name_index = ids["Code"];
  short maxStack = 2;
  short maxLocals = 2;
  int codeLength = 54;
  short exceptionTableLength = 0;
  short codeAttributeCount =1 ;

  
  int stack_map_size;
  BYTE* sm_buf = build_stack_maps_for_lookupswitch(stack_map_size,ids);



  int   attribute_length  = sizeof(maxStack) + sizeof(maxLocals) + sizeof(codeLength) +
    codeLength + sizeof(exceptionTableLength) + sizeof(codeAttributeCount) + stack_map_size;


  size = sizeof(accessFlags) +
         sizeof(nameIndex) +
         sizeof(descriptorIndex) +
         sizeof(attributesCount) +
         6 + 
         attribute_length;

  BYTE* buf = new BYTE[size];
  BYTE* ptr = buf;
  wbytes(ptr , &accessFlags);
  wbytes(ptr , &nameIndex);
  wbytes(ptr , &descriptorIndex);
  wbytes(ptr , &attributesCount);
  
  wbytes(ptr , &attrib_name_index);
  wbytes(ptr , &attribute_length);
  
  wbytes(ptr , &maxStack);
  wbytes(ptr , &maxLocals);
  wbytes(ptr , &codeLength);

  BYTE* code = ptr;

  *(code + 0) = opcode::iload_1;
  *(code + 1) = opcode::lookupswitch;
  int pad = Instruction::calc_pad(1);
  write_bytes(code + 2 + pad, 44, 4);
  const int numberOfPairs = 2;
  const short DOES_NOT_MATTER = 1; // since code is not to be run just make it clear
  write_bytes(code + 2 + pad + 4, numberOfPairs, 4);
  write_bytes(code + 2 + pad + 4 + 4, DOES_NOT_MATTER, 4);
  write_bytes(code + 2 + pad + 4 + 8, 27, 4);
  write_bytes(code + 2 + pad + 4 + 12, DOES_NOT_MATTER, 4);
  write_bytes(code + 2 + pad + 4 + 16, 36, 4);
 
  *(code + 28) = opcode::getstatic;     write_bytes(code + 29, DOES_NOT_MATTER, 2);
  *(code + 31) = opcode::ldc;           *(code + 32) = DOES_NOT_MATTER;
  *(code + 33) = opcode::invokevirtual; write_bytes(code + 34, DOES_NOT_MATTER, 2);
  *(code + 36) = opcode::xreturn;     
  *(code + 37) = opcode::getstatic;     write_bytes(code + 38, DOES_NOT_MATTER, 2);
  *(code + 40) = opcode::ldc;           *(code + 41) = DOES_NOT_MATTER;
  *(code + 42) = opcode::invokevirtual; write_bytes(code + 43, DOES_NOT_MATTER, 2);
  *(code + 45) = opcode::getstatic;     write_bytes(code + 46, DOES_NOT_MATTER, 2);
  *(code + 48) = opcode::ldc;           *(code + 49) = DOES_NOT_MATTER;
  *(code + 50) = opcode::invokevirtual; write_bytes(code + 51, DOES_NOT_MATTER, 2);
  *(code + 53) = opcode::xreturn;

  ptr += 54;
  int zero_excep_size;
  BYTE* excep_buf = build_zero_exceptions(zero_excep_size);
  memcpy(ptr,excep_buf, zero_excep_size);
  ptr += zero_excep_size;
  short codeAttribLen = 1;
  wbytes(ptr,&codeAttribLen);
  memcpy(ptr,sm_buf,stack_map_size);


  return buf;
}


BYTE* build_multi_return_method(int& size, std::unordered_map<std::string, short>& ids)
{



//    public int yeah(int i) {
//      if( i < 10) {
//          System.out.println("less 10");
//          return  0;
//      }
//      else if (i < 100 ) {
//          System.out.println("less 100");
//          return 1;
//      }
//      else {
//          System.out.println("neg");
//          return -1;
//      }
  //  }


  short accessFlags = AccessFlagsComponent::ACC_PUBLIC;
  short nameIndex   = ids["SomeFunc"];
  short descriptorIndex   = ids["()V"];
  short attributesCount = 1;

  short attrib_name_index = ids["Code"];
  short maxStack = 2;
  short maxLocals = 2;
  int codeLength = 42;
  short exceptionTableLength = 0;
  short codeAttributeCount =1 ;

  
  int stack_map_size;
  BYTE* sm_buf = build_stack_maps_for_lookupswitch(stack_map_size,ids);



  int   attribute_length  = sizeof(maxStack) + sizeof(maxLocals) + sizeof(codeLength) +
    codeLength + sizeof(exceptionTableLength) + sizeof(codeAttributeCount) + stack_map_size;


  size = sizeof(accessFlags) +
         sizeof(nameIndex) +
         sizeof(descriptorIndex) +
         sizeof(attributesCount) +
         6 + 
         attribute_length;

  BYTE* buf = new BYTE[size];
  BYTE* ptr = buf;
  wbytes(ptr , &accessFlags);
  wbytes(ptr , &nameIndex);
  wbytes(ptr , &descriptorIndex);
  wbytes(ptr , &attributesCount);
  
  wbytes(ptr , &attrib_name_index);
  wbytes(ptr , &attribute_length);
  
  wbytes(ptr , &maxStack);
  wbytes(ptr , &maxLocals);
  wbytes(ptr , &codeLength);

  
  const int DOES_NOT_MATTER = 1; // since code is not to be run just make it clear
  
  BYTE* code = ptr;
  *(code + 0) = opcode::iload_1;
  *(code + 1) = opcode::bipush;         write_bytes(code + 2, DOES_NOT_MATTER, 1);
  *(code + 3) = opcode::if_icmpge;      write_bytes(code + 4, 16, 2);
  *(code + 6) = opcode::getstatic;      write_bytes(code + 7, DOES_NOT_MATTER,2);
  *(code + 9) = opcode::ldc;            write_bytes(code + 10, DOES_NOT_MATTER,1);
  *(code + 11) = opcode::invokevirtual; write_bytes(code + 12, DOES_NOT_MATTER, 2);
  *(code + 14) = opcode::iconst_0;
  *(code + 15) = opcode::ireturn;
  *(code + 16) = opcode::iload_1;
  *(code + 17) = opcode::bipush;        write_bytes(code + 18, DOES_NOT_MATTER, 1);
  *(code + 19) = opcode::if_icmpge;     write_bytes(code + 20, 32, 2);
  *(code + 22) = opcode::getstatic;     write_bytes(code + 23, DOES_NOT_MATTER, 2);
  *(code + 25) = opcode::ldc;           write_bytes(code + 26, DOES_NOT_MATTER, 1);
  *(code + 27) = opcode::invokevirtual; write_bytes(code + 28, DOES_NOT_MATTER, 2);
  *(code + 30) = opcode::iconst_1;
  *(code + 31) = opcode::ireturn;
  *(code + 32) = opcode::getstatic;     write_bytes(code + 33, DOES_NOT_MATTER, 2);
  *(code + 35) = opcode::ldc;           write_bytes(code + 36, DOES_NOT_MATTER, 1);
  *(code + 37) = opcode::invokevirtual; write_bytes(code + 38, DOES_NOT_MATTER, 2);
  *(code + 40) = opcode::iconst_m1;
  *(code + 41) = opcode::ireturn;

  ptr += 42;
  int zero_excep_size;
  BYTE* excep_buf = build_zero_exceptions(zero_excep_size);
  memcpy(ptr,excep_buf, zero_excep_size);
  ptr += zero_excep_size;
  short codeAttribLen = 1;
  wbytes(ptr,&codeAttribLen);
  memcpy(ptr,sm_buf,stack_map_size);

  
  return buf;
}




TEST_CASE ( "method) = opcode:: [simple] load from buffer")
{
  using namespace frenchroast;
  std::unordered_map<int, std::string> names;
  names[1] = "SomeFunc";
  names[2] = "()V";
  names[3] = "Code";
  names[4] = "StackMapTable";
  

  std::unordered_map<std::string, short> ids;
  for(auto& item : names) {
    ids[item.second] = item.first;
  }
  OpCode::load_from_file( std::getenv("OPCODE_FILE"));
  int size;
  BYTE* buf = build_simple_method(size,ids);
  MethodInfo meth;
  meth.load_from_buffer(buf,names);
  REQUIRE(meth.size_in_bytes() == size);
  REQUIRE(meth[7].address() == 11);
  REQUIRE((meth[7].opcode()  == opcode::xreturn)  );
  REQUIRE(meth[3].offset() == 8);
  REQUIRE(meth[3].address() == 3);
  delete[] buf;

}



TEST_CASE ( "method: [simple]  add instructions at begin")
{
  using namespace frenchroast;
  
  std::unordered_map<int, std::string> names;
  names[1] = "SomeFunc";
  names[2] = "()V";
  names[3] = "Code";
  names[4] = "StackMapTable";
  

  std::unordered_map<std::string, short> ids;
  for(auto& item : names) {
    ids[item.second] = item.first;
  }
  int size;
  BYTE* buf = build_simple_method(size,ids);
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
  
  delete[] buf;
}


TEST_CASE ( "method: [simple]  add instructions at end before call to something()")
{
  using namespace frenchroast;
  std::unordered_map<int, std::string> names;
  names[1] = "SomeFunc";
  names[2] = "()V";
  names[3] = "Code";
  names[4] = "StackMapTable";
  

  std::unordered_map<std::string, short> ids;
  for(auto& item : names) {
    ids[item.second] = item.first;
  }
  
  
  OpCode::load_from_file( std::getenv("OPCODE_FILE"));
  int size;
  BYTE* buf = build_simple_method(size,ids);
  MethodInfo meth;
  meth.load_from_buffer(buf,names);
  
  std::vector<Instruction> list;
  OpCode ops;
  list.push_back(Instruction{ops[opcode::invokestatic], 9});
  REQUIRE(meth[7].address() == 11);
  REQUIRE((meth[7].opcode()  == opcode::xreturn)  );
  REQUIRE(meth[3].offset() == 8);
  meth.add_instructions(6,list,true);// logical intention is since we inserted instructions at the return (address 11) the
  REQUIRE(meth[3].offset() == 11);   // branch should put us at the new instruction, 
                                  
  REQUIRE(meth.size_in_bytes() == size + 3);
  REQUIRE(meth[8].address() == 14);
  REQUIRE((opcode::xreturn == meth[8].opcode()) );

  
  delete[] buf;
}


TEST_CASE ( "method: [simple]  add instructions at end after call to something()")
{
  using namespace frenchroast;
  std::unordered_map<int, std::string> names;
  names[1] = "SomeFunc";
  names[2] = "()V";
  names[3] = "Code";
  names[4] = "StackMapTable";
  

  std::unordered_map<std::string, short> ids;
  for(auto& item : names) {
    ids[item.second] = item.first;
  }
  int size;
  BYTE* buf = build_simple_method(size,ids);
  MethodInfo meth;
  meth.load_from_buffer(buf,names);
  
  std::vector<Instruction> list;
  OpCode ops;
  list.push_back(Instruction{ops[opcode::invokestatic], 9});
  REQUIRE(meth[7].address() == 11);
  REQUIRE((meth[7].opcode()  == opcode::xreturn)  );
  REQUIRE(meth[3].offset() == 8);
  meth.add_instructions(11,list,true);
  REQUIRE(meth[3].offset() == 11);
  REQUIRE(meth[3].offset() + meth[3].address() == 14);
                                  
  REQUIRE(meth.size_in_bytes() == size + 3);
  REQUIRE(meth[8].address() == 14);
  REQUIRE((opcode::xreturn == meth[8].opcode()) );

  
  delete[] buf;
}


TEST_CASE ( "method: [simple]  add instructions at end , always called, not part of if")
{
  using namespace frenchroast;
  
  std::unordered_map<int, std::string> names;
  names[1] = "SomeFunc";
  names[2] = "()V";
  names[3] = "Code";
  names[4] = "StackMapTable";
  

  std::unordered_map<std::string, short> ids;
  for(auto& item : names) {
    ids[item.second] = item.first;
  }

  int size;
  BYTE* buf = build_simple_method(size,ids);
  MethodInfo meth;
  meth.load_from_buffer(buf,names);
  REQUIRE(meth.size_in_bytes() == size );
  std::vector<Instruction> list;
  OpCode ops;
  list.push_back(Instruction{ops[opcode::invokestatic], 9});
  REQUIRE(meth[7].address() == 11);
  REQUIRE((meth[7].opcode()  == opcode::xreturn)  );
  REQUIRE(meth[3].offset() == 8);

  meth.add_instructions(11,list,false);

  REQUIRE(meth[3].offset() == 8);
  REQUIRE(meth[3].offset() + meth[3].address() == 11);
                                  
  REQUIRE(meth.size_in_bytes() == size + 3);
  REQUIRE(meth[8].address() == 14);
  REQUIRE((opcode::xreturn == meth[8].opcode()) );

  
  delete[] buf;
}



TEST_CASE ( "method: [lookupswitch]  add instructions at begin")
{
  using namespace frenchroast;

  std::unordered_map<int, std::string> names;
  names[1] = "SomeFunc";
  names[2] = "()V";
  names[3] = "Code";
  names[4] = "StackMapTable";

  std::unordered_map<std::string, short> ids;
  for(auto& item : names) {
    ids[item.second] = item.first;
  }

  int size;
  BYTE* buf = build_lookupswitch_method(size,ids);
  MethodInfo meth;

  meth.load_from_buffer(buf, names);
  REQUIRE(meth.size_in_bytes() == size );
  std::vector<Instruction> list;
  OpCode ops;
  list.push_back(Instruction{ops[opcode::invokestatic], 9});
  REQUIRE(meth[3].address() == 28);
  REQUIRE(meth.size_in_bytes() == size  );

  meth.add_instructions(0,list,true);

  REQUIRE(meth[2].address() == 3);
  REQUIRE(meth.size_in_bytes() == size + 3 + 1 ); // + 1 since change of address for lookupswitch causes pad change
  REQUIRE(meth[10].address() == 47 +1 );
  REQUIRE(meth[3].is_branch() );
  REQUIRE(meth[3].address() == 4 );
  REQUIRE((meth[7].opcode() == opcode::xgoto));
  REQUIRE(meth[7].address() == 40);
  REQUIRE(meth[7].offset() == 11);
  BYTE* ibuf = meth[3].get_buffer();
  int firstOffset = to_int(ibuf + Instruction::calc_pad(meth[3].address())+ 8 + 4 + 0 * 8 ,4);
  int defaultOffset = to_int(ibuf + Instruction::calc_pad(meth[3].address()), 4);
  REQUIRE(meth[3].address() + firstOffset ==  32);
  REQUIRE(meth[3].address() + defaultOffset ==  47 + 3 +1); // orig target + 3 new bytes + 1 byte adjustment
  REQUIRE((opcode::xreturn == meth[11].opcode()) );

  meth.add_instructions(51,list,false);

  REQUIRE(meth.size_in_bytes() == size + 3 + 1 + 3);
  REQUIRE((meth[7].opcode() == opcode::xgoto));
  REQUIRE(meth[7].address() == 40);
  REQUIRE(meth[7].offset() == 11);

  ibuf = meth[3].get_buffer();
  firstOffset = to_int(ibuf + Instruction::calc_pad(meth[3].address())+ 8 + 4 + 0 * 8 ,4);
  defaultOffset = to_int(ibuf + Instruction::calc_pad(meth[3].address()), 4);
  REQUIRE(meth[3].address() + firstOffset ==  32);
  REQUIRE(meth[3].address() + defaultOffset ==  47 + 3 +1); // orig target + 3 new bytes + 1 byte adjustment
  REQUIRE((opcode::xreturn == meth[12].opcode()) );

  delete[] buf;
}




TEST_CASE ( "method: [simple]   SameFrame  adjusted")
{
  using namespace frenchroast;
  
  std::unordered_map<int, std::string> names;
  names[1] = "SomeFunc";
  names[2] = "()V";
  names[3] = "Code";
  names[4] = "StackMapTable";
  
  std::unordered_map<std::string, short> ids;
  for(auto& item : names) {
    ids[item.second] = item.first;
  }

  int size;
  BYTE* buf = build_simple_method(size,ids);
  MethodInfo meth;
  meth.load_from_buffer(buf, names);
  REQUIRE(meth.frames().size() == 1);
  std::vector<Instruction> list;
  OpCode ops;
  list.push_back(Instruction{ops[opcode::invokestatic], 9});
  REQUIRE(meth[3].address() + meth[3].offset() == 11);
  meth.add_instructions(0,list,true);
  REQUIRE(meth.frames()[0]->offset() == 14);
  REQUIRE(meth[4].address() + meth[4].offset() == 14);

  delete[] buf;
}




TEST_CASE ( "method: [simple]   SameFrame  promoted to SameFrameExtended ")
{
  using namespace frenchroast;
  std::unordered_map<int, std::string> names;
  names[1] = "SomeFunc";
  names[2] = "()V";
  names[3] = "Code";
  names[4] = "StackMapTable";
  
  std::unordered_map<std::string, short> ids;
  for(auto& item : names) {
    ids[item.second] = item.first;
  }

  int size;
  BYTE* buf = build_simple_method(size,ids);
  MethodInfo meth;
  meth.load_from_buffer(buf, names);

  REQUIRE(meth.frames().size() == 1);
  std::vector<Instruction> list;
  OpCode ops;
  for(int idx = 1; idx <= 20; idx++) {
    list.push_back(Instruction{ops[opcode::invokestatic], 9});
  }
  REQUIRE(meth[3].address() + meth[3].offset() == 11);
  
  meth.add_instructions(0,list,true);
  REQUIRE(meth.frames()[0]->offset() == 71);
  REQUIRE(*(meth.frames()[0]) == stackmapframe::same_extended);
  REQUIRE(meth[23].address() + meth[23].offset() == 71);

  delete[] buf;
}


TEST_CASE ( "method: [simple]   multiple SameFrame  adjusted")
{
  using namespace frenchroast;
  std::unordered_map<int, std::string> names;
  names[1] = "SomeFunc";
  names[2] = "()V";
  names[3] = "Code";
  names[4] = "StackMapTable";
  
  std::unordered_map<std::string, short> ids;
  for(auto& item : names) {
    ids[item.second] = item.first;
  }

  int size;
  BYTE* buf = build_lookupswitch_method(size,ids);
  MethodInfo meth;
  meth.load_from_buffer(buf, names);

  
  
  REQUIRE(meth.frames().size() == 3);
  std::vector<Instruction> list;
  OpCode ops;
  list.push_back(Instruction{ops[opcode::invokestatic], 9});
  
  meth.add_instructions(0,list,true);
  REQUIRE(meth.frames()[0]->offset() == 32);
  REQUIRE(meth.frames()[1]->offset() == 10);
  REQUIRE(meth.frames()[2]->offset() == 7);

  delete[] buf;
}



// namespace MethodInfo {
//   std::vector<int> get_targets(Instruction& inst);
// }

TEST_CASE ( "method: [simple]   multiple SameFrame  adjusted, with return in middle of switch")
{
  using namespace frenchroast;

  std::unordered_map<int, std::string> names;
  names[1] = "SomeFunc";
  names[2] = "()V";
  names[3] = "Code";
  names[4] = "StackMapTable";
  
  std::unordered_map<std::string, short> ids;
  for(auto& item : names) {
    ids[item.second] = item.first;
  }

  int size;
  BYTE* buf = build_lookupswitch_return_method(size,ids);
  MethodInfo meth;
  meth.load_from_buffer(buf, names);

  REQUIRE(meth.frames().size() == 3);
  
  std::vector<Instruction> list;
  OpCode ops;
  list.push_back(Instruction{ops[opcode::invokestatic], 9});
  REQUIRE(meth.get_targets(meth[2])[0] == 45);
  REQUIRE(meth.get_targets(meth[2])[1] == 28);
  REQUIRE(meth.get_targets(meth[2])[2] == 37);
  meth.add_instructions(0,list,true);

  //meth.add_instructions(61,list,false);

  REQUIRE(meth.get_targets(meth[3])[0] == 49);
  REQUIRE(meth.get_targets(meth[3])[1] == 32 );
  REQUIRE(meth.get_targets(meth[3])[2] == 41);

  meth.add_instructions(40,list,true);
  REQUIRE(meth.get_targets(meth[3])[0] == 52);
  REQUIRE(meth.get_targets(meth[3])[1] == 32);
  REQUIRE(meth.get_targets(meth[3])[2] == 44);

  delete[] buf;
}


TEST_CASE ( "method : get_return_addresses")
{

  std::unordered_map<int, std::string> names;
  names[1] = "SomeFunc";
  names[2] = "()V";
  names[3] = "Code";
  names[4] = "StackMapTable";
  
  std::unordered_map<std::string, short> ids;
  for(auto& item : names) {
    ids[item.second] = item.first;
  }

  int size;
  BYTE* buf = build_multi_return_method(size,ids);
  MethodInfo meth;
  meth.load_from_buffer(buf, names);

  std::vector<int> rlist = meth.get_return_addresses();
  REQUIRE(rlist.size() == 3);
  REQUIRE(meth[7].address() == rlist[2]);
}
