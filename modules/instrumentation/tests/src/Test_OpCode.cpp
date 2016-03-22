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

TEST_CASE ("load")
{
    using namespace frenchroast;
  OpCode ops; 
  OpCode::load( std::getenv("OPCODE_FILE"));
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
