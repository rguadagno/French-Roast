// copyright (c) 2017 Richard Guadagno
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
//    along with French-Roast.  If not, see <http://www.gnu.org/licenses/>.
//

#include <vector>
#include <iostream>
#include <sstream>
#include "catch.hpp"
#include "Descriptor.h"

using namespace frenchroast::monitor;


TEST_CASE("Descriptor method only")
{
  Descriptor dsc{"funcA:(I)V"};
  std::stringstream ss;
  ss << dsc;
  REQUIRE(ss.str() == "funcA:(int):void");
  REQUIRE(dsc.class_name() == "");
  REQUIRE(dsc.method_name() == "funcA:(int):void");
}


TEST_CASE("operator<<,  Descriptor")
{
  Descriptor dsc{"Lmypackage/SomeClass;::funcA:(I)V"};
  std::stringstream ss;
  ss << dsc;
  REQUIRE(ss.str() == "mypackage.SomeClass::funcA:(int):void");
  REQUIRE(dsc.class_name() == "mypackage.SomeClass");
  REQUIRE(dsc.method_name() == "funcA:(int):void");
}


TEST_CASE("operator>>,  Descriptor")
{
  Descriptor dsc{};
  "mypackage.SomeClass::funcA:(int):void" >> dsc;
  REQUIRE(dsc.class_name() == "mypackage.SomeClass");
  REQUIRE(dsc.method_name() == "funcA:(int):void");


  "funcA:(int):void" >> dsc;
  REQUIRE(dsc.class_name() == "");
  REQUIRE(dsc.method_name() == "funcA:(int):void");

  
}

TEST_CASE("operator==,  Descriptor")
{

  Descriptor dsc1{"mypackage/SomeClass;::funcA:(I)V"};
  Descriptor dsc2{"mypackage/SomeClass;::funcA:(I)V"};
  REQUIRE(dsc1 == dsc2);
}

TEST_CASE("operator=,  Descriptor")
{

  Descriptor dsc1{"Lmypackage/SomeClass;::funcA:(I)V"};
  Descriptor dsc2{};
  dsc2 = dsc1;
  REQUIRE(dsc1 == dsc2);
}


