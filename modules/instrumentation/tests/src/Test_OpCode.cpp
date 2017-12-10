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
#include "OpCode.h"

TEST_CASE ("bad file name")
{
  using namespace frenchroast;
  REQUIRE_THROWS_WITH(OpCode::load_from_file("junk"), "cannot open file: junk");
}

TEST_CASE ("bad load line")
{
  using namespace frenchroast;
  OpCode ops; 
  REQUIRE_THROWS_WITH( OpCode::load("junk line"), "bad opcode line: junk line");
}

TEST_CASE ("good load line")
{
  using namespace frenchroast;
  OpCode ops; 
  OpCode::load("dcmpg           <152> 1");
  REQUIRE(ops[152].get_size() == 1);
  OpCode::load("dup2_x2         <94>  1");
  REQUIRE(ops[94].get_size() == 1);
  OpCode::load("ifeq            <153> 3 <branch>    ");
  REQUIRE(ops[153].get_size() == 3);
  OpCode::load("lookupswitch    <171> * <branch>");
  REQUIRE(ops[171].is_dynamic());
  OpCode::load("invokeinterface <185> 5 <raw>");
  REQUIRE(ops[185].get_size() == 5);
}




TEST_CASE ("load")
{
  using namespace frenchroast;
  OpCode ops; 
  OpCode::load_from_file( std::getenv("OPCODE_FILE"));
  REQUIRE(ops[1].get_size() == 1 );

  REQUIRE(ops[171].get_name() == "lookupswitch" );
  REQUIRE(ops[171].get_size() == 0 );
  REQUIRE(ops[171].is_dynamic() == true );
  REQUIRE(ops[171].is_branch() == true );

  REQUIRE(ops[25].get_name() == "aload" );
  REQUIRE(ops[25].is_dynamic() == false );
  REQUIRE(ops[25].get_size() == 2 );
  REQUIRE(ops[25].is_branch() == false );
}

