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


using namespace frenchroast;


TEST_CASE ("Method: empty")
{
  using namespace frenchroast;
  
  // OpCode::load_from_file( std::getenv("OPCODE_FILE"));

  MethodInfo method;
  REQUIRE(method.size_in_bytes() == 8 + attribute::HeaderSize + 2 + 2 + 4 + 2 + 2);
}

TEST_CASE ("Method: access flags")
{
  using namespace frenchroast;
  
  // OpCode::load_from_file( std::getenv("OPCODE_FILE"));

  MethodInfo method;
  method.set_access_flags(AccessFlagsComponent::ACC_PUBLIC);
  REQUIRE(method.get_access_flags() == AccessFlagsComponent::ACC_PUBLIC);

}



  
