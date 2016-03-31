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
#include "ClassFileComponent.h"
#include "Util.h"
#include "Instruction.h"
#include "OpCodeConst.h"
#include "Method.h"
#include "StackMapFrame.h"
#include "FrameConst.h"


using namespace frenchroast;

BYTE* build_simple_method(int& size)
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
  size = 12;
  BYTE* buf = new BYTE[size + 8];
  short maxStack = 2;
  short maxLocals = 2;
  int codeLength = size;
  write_big_e_bytes(buf, &maxStack);
  write_big_e_bytes(buf +2 , &maxLocals);
  write_big_e_bytes(buf +4 , &codeLength);

  BYTE* code = buf + 8;
  *(code + 0) = opcode::iload_1;
  *(code + 1) = opcode::bipush; *(code + 2) = 10;
  *(code + 3) = opcode::if_icmpge; write_bytes(code + 4, 8, 2);
  *(code + 6) = opcode::aload_0;
  *(code + 7) = opcode::iload_1;
  *(code + 8) = opcode::invokevirtual; write_bytes(code + 9, 7, 2);
  *(code + 11) = opcode::xreturn;
  return buf;
}


BYTE* build_stack_maps_for_simple()
{
  BYTE* buf = new BYTE[1];
  *buf = 11;
  return buf;
}

BYTE* build_stack_maps_for_lookupswitch()
{
  BYTE* buf = new BYTE[3];
  *(buf + 0) = 28;
  *(buf + 1) = 10;
  *(buf + 2) = 7;
  return buf;
}



BYTE* build_lookupswitch_method(int& size)
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

  size = 48;
  BYTE* buf = new BYTE[size + 8];
  memset(buf,0,size);
  short maxStack = 2;
  short maxLocals = 2;
  int codeLength = size;
  write_big_e_bytes(buf, &maxStack);
  write_big_e_bytes(buf +2 , &maxLocals);
  write_big_e_bytes(buf +4 , &codeLength);

  BYTE* code = buf + 8;
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
  
  return buf;
}

BYTE* build_lookupswitch_return_method(int& size)
{
/*
   ----------- method ------------------

public void something(int x) {
	switch(x) {
	case 10:   
	    System.out.println("okok");     
	    return;
    
	case 20
	    System.out.println("xxx");
	    break;
	}
	System.out.println("NOTHING");
    }

*/

  size = 54;
  BYTE* buf = new BYTE[size + 8];
  memset(buf,0,size);
  short maxStack = 2;
  short maxLocals = 2;
  int codeLength = size;
  write_big_e_bytes(buf, &maxStack);
  write_big_e_bytes(buf +2 , &maxLocals);
  write_big_e_bytes(buf +4 , &codeLength);

  BYTE* code = buf + 8;
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
  
  return buf;
}



TEST_CASE ( "method: [simple] load from buffer")
{
  using namespace frenchroast;
  

  int size;
  BYTE* buf = build_simple_method(size);
  Method meth;
  meth.load_from_buffer(buf);
  REQUIRE(meth.size_in_bytes() == size + 8);
  std::vector<Instruction> list;
  OpCode ops;
  REQUIRE(meth[7].address() == 11);
  REQUIRE(meth[7].opcode()  == opcode::xreturn  );
  REQUIRE(meth[3].offset() == 8);
  REQUIRE(meth[3].address() == 3);

  
  delete[] buf;
}

TEST_CASE ( "method: [simple]  add instructions at begin")
{
  using namespace frenchroast;
  

  int size;
  BYTE* buf = build_simple_method(size);
  Method meth;
  meth.load_from_buffer(buf);
  REQUIRE(meth.size_in_bytes() == size + 8);
  std::vector<Instruction> list;
  OpCode ops;
  list.push_back(Instruction{ops[opcode::invokestatic], 9});
  REQUIRE(meth[7].address() == 11);
  REQUIRE(meth[7].opcode()  == opcode::xreturn  );
  REQUIRE(meth[3].offset() == 8);
  REQUIRE(meth.size_in_bytes() == size + 8);

  meth.add_instructions(0,list,true);

  REQUIRE(meth.size_in_bytes() == size + 8 + 3);
  REQUIRE(meth[8].address() == 14);
  REQUIRE(opcode::xreturn == meth[8].opcode() );
  REQUIRE(meth[4].address() == 6);
  REQUIRE(meth[4].offset() == 8);
  
  delete[] buf;
}

TEST_CASE ( "method: [simple]  add instructions at end before call to something()")
{
  using namespace frenchroast;
  

  int size;
  BYTE* buf = build_simple_method(size);
  Method meth;
  meth.load_from_buffer(buf);
  REQUIRE(meth.size_in_bytes() == size + 8);
  std::vector<Instruction> list;
  OpCode ops;
  list.push_back(Instruction{ops[opcode::invokestatic], 9});
  REQUIRE(meth[7].address() == 11);
  REQUIRE(meth[7].opcode()  == opcode::xreturn  );
  REQUIRE(meth[3].offset() == 8);
  meth.add_instructions(6,list,true);// logical intention is since we inserted instructions at the return (address 11) the
  REQUIRE(meth[3].offset() == 11);   // branch should put us at the new instruction, 
                                  
  REQUIRE(meth.size_in_bytes() == size + 8 + 3);
  REQUIRE(meth[8].address() == 14);
  REQUIRE(opcode::xreturn == meth[8].opcode() );

  
  delete[] buf;
}


TEST_CASE ( "method: [simple]  add instructions at end after call to something()")
{
  using namespace frenchroast;
  

  int size;
  BYTE* buf = build_simple_method(size);
  Method meth;
  meth.load_from_buffer(buf);
  REQUIRE(meth.size_in_bytes() == size + 8);
  std::vector<Instruction> list;
  OpCode ops;
  list.push_back(Instruction{ops[opcode::invokestatic], 9});
  REQUIRE(meth[7].address() == 11);
  REQUIRE(meth[7].opcode()  == opcode::xreturn  );
  REQUIRE(meth[3].offset() == 8);
  meth.add_instructions(11,list,true);
  REQUIRE(meth[3].offset() == 11);
  REQUIRE(meth[3].offset() + meth[3].address() == 14);
                                  
  REQUIRE(meth.size_in_bytes() == size + 8 + 3);
  REQUIRE(meth[8].address() == 14);
  REQUIRE(opcode::xreturn == meth[8].opcode() );

  
  delete[] buf;
}


TEST_CASE ( "method: [simple]  add instructions at end , always called, not part of if")
{
  using namespace frenchroast;
  

  int size;
  BYTE* buf = build_simple_method(size);
  Method meth;
  meth.load_from_buffer(buf);
  REQUIRE(meth.size_in_bytes() == size + 8);
  std::vector<Instruction> list;
  OpCode ops;
  list.push_back(Instruction{ops[opcode::invokestatic], 9});
  REQUIRE(meth[7].address() == 11);
  REQUIRE(meth[7].opcode()  == opcode::xreturn  );
  REQUIRE(meth[3].offset() == 8);

  meth.add_instructions(11,list,false);

  REQUIRE(meth[3].offset() == 8);
  REQUIRE(meth[3].offset() + meth[3].address() == 11);
                                  
  REQUIRE(meth.size_in_bytes() == size + 8 + 3);
  REQUIRE(meth[8].address() == 14);
  REQUIRE(opcode::xreturn == meth[8].opcode() );

  
  delete[] buf;
}



TEST_CASE ( "method: [lookupswitch]  add instructions at begin")
{
  using namespace frenchroast;
  

  int size;
  BYTE* buf = build_lookupswitch_method(size);
  Method meth;
  meth.load_from_buffer(buf);
  REQUIRE(meth.size_in_bytes() == size + 8);
  std::vector<Instruction> list;
  OpCode ops;
  list.push_back(Instruction{ops[opcode::invokestatic], 9});
  REQUIRE(meth[3].address() == 28);
  REQUIRE(meth.size_in_bytes() == size + 8 );

  meth.add_instructions(0,list,true);

  REQUIRE(meth[2].address() == 3);
  REQUIRE(meth.size_in_bytes() == size + 8 + 3 + 1 ); // + 1 since change of address for lookupswitch causes pad change
  REQUIRE(meth[10].address() == 47 +1 );
  REQUIRE(meth[3].is_branch() );
  REQUIRE(meth[3].address() == 4 );
  REQUIRE(meth[7].opcode() == opcode::xgoto);
  REQUIRE(meth[7].address() == 40);
  REQUIRE(meth[7].offset() == 11);
  BYTE* ibuf = meth[3].get_buffer();
  int firstOffset = to_int(ibuf + Instruction::calc_pad(meth[3].address())+ 8 + 4 + 0 * 8 ,4);
  int defaultOffset = to_int(ibuf + Instruction::calc_pad(meth[3].address()), 4);
  REQUIRE(meth[3].address() + firstOffset ==  32);
  REQUIRE(meth[3].address() + defaultOffset ==  47 + 3 +1); // orig target + 3 new bytes + 1 byte adjustment
  REQUIRE(opcode::xreturn == meth[11].opcode() );

  meth.add_instructions(51,list,false);

  REQUIRE(meth.size_in_bytes() == size + 8 + 3 + 1 + 3);
  REQUIRE(meth[7].opcode() == opcode::xgoto);
  REQUIRE(meth[7].address() == 40);
  REQUIRE(meth[7].offset() == 11);

  ibuf = meth[3].get_buffer();
  firstOffset = to_int(ibuf + Instruction::calc_pad(meth[3].address())+ 8 + 4 + 0 * 8 ,4);
  defaultOffset = to_int(ibuf + Instruction::calc_pad(meth[3].address()), 4);
  REQUIRE(meth[3].address() + firstOffset ==  32);
  REQUIRE(meth[3].address() + defaultOffset ==  47 + 3 +1); // orig target + 3 new bytes + 1 byte adjustment
  REQUIRE(opcode::xreturn == meth[12].opcode() );


  
  delete[] buf;
}


TEST_CASE ( "method: [simple]   SameFrame  adjusted")
{
  using namespace frenchroast;
  

  int size;
  BYTE* buf = build_simple_method(size);
  Method meth;
  meth.load_from_buffer(buf);
  BYTE* sbuf = build_stack_maps_for_simple();
  std::vector<StackMapFrame*> frames = load_frames_from_buffer(1, sbuf);
  REQUIRE(frames.size() == 1);
  
  std::vector<Instruction> list;
  OpCode ops;
  list.push_back(Instruction{ops[opcode::invokestatic], 9});
  REQUIRE(meth[3].address() + meth[3].offset() == 11);
  
  meth.add_instructions(0,list,true);
  meth.adjust_frames(frames);
  REQUIRE(frames[0]->offset() == 14);
  REQUIRE(meth[4].address() + meth[4].offset() == 14);

  delete[] buf;
}

TEST_CASE ( "method: [simple]   SameFrame  promoted to SameFrameExtended ")
{
  using namespace frenchroast;
  

  int size;
  BYTE* buf = build_simple_method(size);
  Method meth;
  meth.load_from_buffer(buf);
  BYTE* sbuf = build_stack_maps_for_simple();
  std::vector<StackMapFrame*> frames = load_frames_from_buffer(1, sbuf);
  REQUIRE(frames.size() == 1);
  std::vector<Instruction> list;
  OpCode ops;
  for(int idx = 1; idx <= 20; idx++) {
    list.push_back(Instruction{ops[opcode::invokestatic], 9});
  }
  REQUIRE(meth[3].address() + meth[3].offset() == 11);
  
  meth.add_instructions(0,list,true);
  meth.adjust_frames(frames);
  REQUIRE(frames[0]->offset() == 71);
  REQUIRE(*(frames[0]) == stackmapframe::same_extended);
  REQUIRE(meth[23].address() + meth[23].offset() == 71);

  delete[] buf;
}


TEST_CASE ( "method: [simple]   multiple SameFrame  adjusted")
{
  using namespace frenchroast;
  

  int size;
  BYTE* buf = build_lookupswitch_method(size);
  Method meth;
  meth.load_from_buffer(buf);
  BYTE* sbuf = build_stack_maps_for_lookupswitch();
  std::vector<StackMapFrame*> frames = load_frames_from_buffer(3, sbuf);
  REQUIRE(frames.size() == 3);
  
  std::vector<Instruction> list;
  OpCode ops;
  list.push_back(Instruction{ops[opcode::invokestatic], 9});
  
  meth.add_instructions(0,list,true);
  meth.adjust_frames(frames);
  REQUIRE(frames[0]->offset() == 32);
  REQUIRE(frames[1]->offset() == 10);
  REQUIRE(frames[2]->offset() == 7);


  delete[] buf;
}

namespace frenchroast {
  std::vector<int> get_targets(Instruction& inst);
}

TEST_CASE ( "method: [simple]   multiple SameFrame  adjusted, with return in middle of switch")
{
  using namespace frenchroast;
  

  int size;
  BYTE* buf = build_lookupswitch_return_method(size);
  Method meth;
  meth.load_from_buffer(buf);
  BYTE* sbuf = build_stack_maps_for_lookupswitch();
  std::vector<StackMapFrame*> frames = load_frames_from_buffer(3, sbuf);
  REQUIRE(frames.size() == 3);
  
  std::vector<Instruction> list;
  OpCode ops;
  list.push_back(Instruction{ops[opcode::invokestatic], 9});
  REQUIRE(get_targets(meth[2])[0] == 45);
  REQUIRE(get_targets(meth[2])[1] == 28);
  REQUIRE(get_targets(meth[2])[2] == 37);
  meth.add_instructions(0,list,true);

  //meth.add_instructions(61,list,false);

  REQUIRE(get_targets(meth[3])[0] == 49);
  REQUIRE(get_targets(meth[3])[1] == 32 );
  REQUIRE(get_targets(meth[3])[2] == 41);

  meth.add_instructions(40,list,true);
  REQUIRE(get_targets(meth[3])[0] == 52);
  REQUIRE(get_targets(meth[3])[1] == 32 );
  REQUIRE(get_targets(meth[3])[2] == 44);

  delete[] buf;
}



