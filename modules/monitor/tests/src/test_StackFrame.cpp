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
#include "StackFrame.h"


using namespace frenchroast::monitor;


TEST_CASE("StackFrame(Descriptor)")
{
  Descriptor dsc{"mypackage/SomeClass::funcA:(I)V"};
  StackFrame sf{dsc};
  REQUIRE(sf.get_name() == "mypackage.SomeClass::funcA:(int):void");
}

TEST_CASE("StackFrame(Descriptor) move")
{
  StackFrame sf{Descriptor{"mypackage/SomeClass::funcA:(I)V"}};
  REQUIRE(sf.get_name() == "mypackage.SomeClass::funcA:(int):void");
  REQUIRE(sf.get_monitor_count() == 0);
}

TEST_CASE("StackFrame(Descriptor,1) ")
{
  StackFrame sf{Descriptor{"mypackage/SomeClass::funcA:(I)V"}, 1};
  REQUIRE(sf.get_name() == "mypackage.SomeClass::funcA:(int):void");
  REQUIRE(sf.get_monitor_count() == 1);  
}


TEST_CASE("StackFrame(Descriptor,1) operator <<")
{
  StackFrame sf{Descriptor{"mypackage/SomeClass::funcA:(I)V"}, 1};
  std::stringstream ss;
  ss << sf;
  REQUIRE( ss.str() == "mypackage.SomeClass::funcA:(int):void<end-method>1");
}

TEST_CASE("StackFrame(Descriptor,1) operator >>")
{
  StackFrame sf{};
  std::string line = "mypackage.SomeClass::funcA:(int):void<end-method>1";
  line >> sf;
  REQUIRE(sf.get_name() == "mypackage.SomeClass::funcA:(int):void");
  REQUIRE(sf.get_monitor_count() == 1);  
}

