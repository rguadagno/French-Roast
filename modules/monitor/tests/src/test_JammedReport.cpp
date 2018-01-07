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
#include "JammedReport.h"
#include "StackTrace.h"
#include "helper.h"

using namespace frenchroast::monitor;

TEST_CASE("jammedRpeort: construction")
{
  StackTrace owner = frenchroast::testing::build_trace({"mypackage.SomeClass::funcA:(int):void<end-method>1"});
  StackTrace waiter = frenchroast::testing::build_trace({"mypackage.SomeClass::funcB:(int):void<end-method>0"});

  JammedReport jr{waiter,owner};
  jr.add_monitor("java/lang/Object");  
  REQUIRE(jr.key() == waiter.key() + owner.key());
  
  REQUIRE((jr.owner() == owner));
  REQUIRE((jr.waiter() == waiter));
  REQUIRE(jr.monitors().size() == 1);
  REQUIRE(jr.monitors()[0]  == "java/lang/Object");
}


TEST_CASE("jammedRpeort: operator <<")
{
  StackTrace owner = frenchroast::testing::build_trace({"mypackage.SomeClass::funcA:(int):void<end-method>1"});
  StackTrace waiter = frenchroast::testing::build_trace({"mypackage.SomeClass::funcB:(int):void<end-method>0"});

  JammedReport jr{waiter,owner};
  jr.add_monitor("java/lang/Object");
  jr.add_monitor("SomeOtherClass");

  std::stringstream ss;
  ss << jr;
  REQUIRE(ss.str() == "SomeOtherClass<end-monitor>java/lang/Object<end-monitor><end-jmonitors><end-thread-name>1<end-monitor><end-monitors>mypackage.SomeClass::funcA:(int):void<end-method>1<end-frame><end-owner><end-thread-name>0<end-monitor><end-monitors>mypackage.SomeClass::funcB:(int):void<end-method>0<end-frame>");
}

TEST_CASE("jammedRpeort: operator ==")
{
  StackTrace owner = frenchroast::testing::build_trace({"mypackage.SomeClass::funcA:(int):void<end-method>1"});
  StackTrace waiter = frenchroast::testing::build_trace({"mypackage.SomeClass::funcB:(int):void<end-method>0"});

  JammedReport jr{waiter,owner};
  jr.add_monitor("java/lang/Object");
  jr.add_monitor("SomeOtherClass");

  JammedReport jr2{waiter,owner};
  jr2.add_monitor("java/lang/Object");
  jr2.add_monitor("SomeOtherClass");
  REQUIRE(jr == jr2);
  
}


TEST_CASE("jammedRpeort: operator >>")
{
  StackTrace owner = frenchroast::testing::build_trace({"mypackage.SomeClass::funcA:(int):void<end-method>1"});
  StackTrace waiter = frenchroast::testing::build_trace({"mypackage.SomeClass::funcB:(int):void<end-method>0"});

  JammedReport jr{waiter,owner};
  jr.add_monitor("SomeOtherClass");
  jr.add_monitor("java/lang/Object");

  std::stringstream ss;
  ss << jr;
  JammedReport rpt{};
  //  "SomeOtherClass<end-monitor>java/lang/Object<end-monitor><end-jmonitors><end-thread-name>1<end-monitor><end-monitors>mypackage.SomeClass::funcA:(int):void<end-method>1<end-frame><end-owner><end-thread-name>0<end-monitor><end-monitors>mypackage.SomeClass::funcB:(int):void<end-method>0<end-frame>" >> rpt;
  ss.str() >> rpt;

  REQUIRE(rpt == jr);
}

