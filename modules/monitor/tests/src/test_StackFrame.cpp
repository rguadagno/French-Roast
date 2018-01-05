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

TEST_CASE("StackFrame(Descriptor) operator ==")
{
  Descriptor dsc{"mypackage/SomeClass::funcA:(I)V"};
  StackFrame sf1{dsc,1};
  StackFrame sf2{dsc,1};
  REQUIRE(sf1 == sf2);
  StackFrame sf3{dsc,2};
  REQUIRE(sf1 == sf3);

  
}

TEST_CASE("StackFrame(Descriptor) operator !=")
{
  Descriptor dsc{"mypackage/SomeClass::funcA:(I)V"};
  StackFrame sf1{dsc};
  Descriptor dsc2{"mypackage/SomeClass::funcB:(I)V"};
  StackFrame sf2{dsc2};
  REQUIRE(sf1 != sf2);
}

TEST_CASE("StackFrame(Descriptor,1) string conversion operator")
{
  StackFrame sf{};
  std::string line = "mypackage.SomeClass::funcA:(int):void<end-method>1";
  line >> sf;
  std::string sout = sf;
  REQUIRE(sout  == "mypackage.SomeClass::funcA:(int):void");
}

TEST_CASE("StackFrame operator << vector")
{
  StackFrame sf1{Descriptor{"mypackage/SomeClass::funcA:(I)V"}, 1};
  StackFrame sf2{Descriptor{"mypackage/SomeClass::funcB:(I)V"}, 0};

  std::vector<StackFrame> frames{sf1,sf2};
  
  std::stringstream ss;
  ss << frames;
  REQUIRE( ss.str() == "mypackage.SomeClass::funcA:(int):void<end-method>1<end-frame>mypackage.SomeClass::funcB:(int):void<end-method>0<end-frame>");
}

TEST_CASE("StackFrame operator >> vector")
{
  StackFrame sf1{Descriptor{"mypackage/SomeClass::funcA:(I)V"}, 1};
  StackFrame sf2{Descriptor{"mypackage/SomeClass::funcB:(I)V"}, 0};

  std::vector<StackFrame> frames;
  "mypackage.SomeClass::funcA:(int):void<end-method>1<end-frame>mypackage.SomeClass::funcB:(int):void<end-method>0<end-frame>" >> frames;

  REQUIRE(frames.size() == 2);
  REQUIRE(frames[0] == sf1);
  REQUIRE(frames[1] == sf2);

}


