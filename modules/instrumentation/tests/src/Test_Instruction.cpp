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
#include "Instruction.h"
#include "OpCodeConst.h"




TEST_CASE ( "default cons")
{
  using namespace frenchroast;
  Instruction instr;
  REQUIRE(instr.get_size() == 0);
}


TEST_CASE ("pad")
{
    using namespace frenchroast;

    REQUIRE(Instruction::calc_pad(0) == 3 );
    REQUIRE(Instruction::calc_pad(1) == 2 );
    REQUIRE(Instruction::calc_pad(2) == 1 );
    REQUIRE(Instruction::calc_pad(3) == 0 );
    REQUIRE(Instruction::calc_pad(4) == 3 );
    REQUIRE(Instruction::calc_pad(5) == 2 );
    REQUIRE(Instruction::calc_pad(6) == 1 );
    REQUIRE(Instruction::calc_pad(7) == 0 );
}


TEST_CASE ("load_iload0")
{
  using namespace frenchroast;
  const int EXTRA_ROOM = 10;
  BYTE buf[EXTRA_ROOM];
  buf[0] = opcode::iload_0;
  
  Instruction instr;
  int loaded;
  instr.load_from_buffer(buf,0,loaded);

  REQUIRE(instr.get_size() == 1);
  REQUIRE(instr.is_branch() == false);
  
}


TEST_CASE ("lookupswitch start at 1")
{
  using namespace frenchroast;
  const int EXTRA_ROOM = 10;

  BYTE buf[100];
  memset(buf,0,sizeof(buf));
  
  buf[0] = opcode::lookupswitch;
  buf[1] = 0;
  buf[2] = 0;
  int defaultoffset = 50;
  write_big_e_bytes(&buf[3], &defaultoffset);
  int pairscount = 2;
  write_big_e_bytes(&buf[7], &pairscount);
  int lookup1 = 10;
  int offset1 = 28;
  int lookup2 = 20;
  int offset2 = 39;

  write_big_e_bytes(&buf[11], &lookup1);
  write_big_e_bytes(&buf[15], &offset1);
  write_big_e_bytes(&buf[19], &lookup1);
  write_big_e_bytes(&buf[23], &offset2);
  
  Instruction instr;
  int loaded;
  REQUIRE(instr.load_from_buffer(buf,1,loaded) == 27);

  REQUIRE(instr.get_size() == 27);
  REQUIRE(instr.is_branch() == true);

  BYTE tobuf[100];
  memset(tobuf,0,sizeof(tobuf));
  instr.load_to_buffer(tobuf);
  REQUIRE(memcmp(tobuf,buf,sizeof(tobuf)) == 0);

  int insertedAt = 0;
  int byteCount = 3;
  instr.adjust(0,3);
  REQUIRE(instr.get_size() == 28);
  
  
}

TEST_CASE ("lookupswitch start at 0")
{
  using namespace frenchroast;
  const int EXTRA_ROOM = 10;

  BYTE buf[100];
  memset(buf,0,sizeof(buf));
  
  buf[0] = opcode::lookupswitch;
  buf[1] = 0;
  buf[2] = 0;
  buf[3] = 0;
  int defaultoffset = 50;
  write_big_e_bytes(&buf[4], &defaultoffset);
  int pairscount = 2;
  write_big_e_bytes(&buf[8], &pairscount);
  int lookup1 = 10;
  int offset1 = 28;
  int lookup2 = 20;
  int offset2 = 39;

  write_big_e_bytes(&buf[12], &lookup1);
  write_big_e_bytes(&buf[16], &offset1);
  write_big_e_bytes(&buf[20], &lookup1);
  write_big_e_bytes(&buf[24], &offset2);
  
  Instruction instr;
  int loaded;
  REQUIRE(instr.load_from_buffer(buf,0,loaded) == 28);

  REQUIRE(instr.get_size() == 28);
  REQUIRE(instr.is_branch() == true);

  BYTE tobuf[100];
  memset(tobuf,0,sizeof(tobuf));
  instr.load_to_buffer(tobuf);
  REQUIRE(memcmp(tobuf,buf,sizeof(tobuf)) == 0);

  int insertedAt = 0;
  int byteCount = 3;
  instr.adjust(0,3);
  REQUIRE(instr.get_size() == 25);
  
  
}





  
