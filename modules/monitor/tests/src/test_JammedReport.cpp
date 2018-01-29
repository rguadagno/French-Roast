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
#include <algorithm>
#include <unordered_map>
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
  REQUIRE(jr.key() == waiter.key() + owner.key());
  
  REQUIRE((jr.owner() == owner));
  REQUIRE((jr.waiter() == waiter));
}

TEST_CASE("jammedRpeort: add_monitor")
{
  StackTrace owner = frenchroast::testing::build_trace({"mypackage.SomeClass::funcA:(int):void<end-method>1"});
  StackTrace waiter = frenchroast::testing::build_trace({"mypackage.SomeClass::funcB:(int):void<end-method>0"});

  JammedReport jr{waiter,owner};
  jr.add_monitor("Ljava/lang/Object");  

  REQUIRE(jr.monitors().size() == 1);
  REQUIRE(jr.monitors()[0]  == "java.lang.Object");

  jr.add_monitor("Ljava/lang/Object");  
  REQUIRE(jr.monitors().size() == 1);
  REQUIRE(jr.monitors()[0]  == "java.lang.Object");
  jr.add_monitor("LSomeClass");
  auto mons = jr.monitors();
  REQUIRE(mons.size() == 2);
  std::sort(mons.begin(), mons.end());
  REQUIRE(jr.monitors().size() == 2);
  REQUIRE(jr.monitors()[0]  == "SomeClass");
  REQUIRE(jr.monitors()[1]  == "java.lang.Object");
  
}


TEST_CASE("jammedRpeort: operator <<")
{
  StackTrace owner = frenchroast::testing::build_trace({"mypackage.SomeClass::funcA:(int):void<end-method>1"});
  StackTrace waiter = frenchroast::testing::build_trace({"mypackage.SomeClass::funcB:(int):void<end-method>0"});

  JammedReport jr{waiter,owner};
  jr.add_monitor("Ljava/lang/Object");
  jr.add_monitor("LSomeOtherClass");

  std::stringstream ss;
  ss << jr;
  REQUIRE(ss.str() == "SomeOtherClass<end-monitor>java.lang.Object<end-monitor><end-jmonitors><end-thread-name>1<end-monitor><end-monitors>mypackage.SomeClass::funcA:(int):void<end-method>1<end-frame><end-owner><end-thread-name>0<end-monitor><end-monitors>mypackage.SomeClass::funcB:(int):void<end-method>0<end-frame>");
}

TEST_CASE("jammedRpeort: operator ==")
{
  StackTrace owner = frenchroast::testing::build_trace({"mypackage.SomeClass::funcA:(int):void<end-method>1"});
  StackTrace waiter = frenchroast::testing::build_trace({"mypackage.SomeClass::funcB:(int):void<end-method>0"});

  JammedReport jr{waiter,owner};
  jr.add_monitor("Ljava/lang/Object");
  jr.add_monitor("LSomeOtherClass");

  JammedReport jr2{waiter,owner};
  jr2.add_monitor("Ljava/lang/Object");
  jr2.add_monitor("LSomeOtherClass");
  REQUIRE(jr == jr2);
  
}


TEST_CASE("jammedRpeort: operator unordered_map<string , JammedReport> == unordered_map<JammedReport>")
{
  StackTrace owner = frenchroast::testing::build_trace({"mypackage.SomeClass::funcA:(int):void<end-method>1"});
  StackTrace waiter = frenchroast::testing::build_trace({"mypackage.SomeClass::funcB:(int):void<end-method>0"});

  JammedReport jr{waiter,owner};
  jr.add_monitor("java/lang/Object");
  jr.add_monitor("SomeOtherClass");

  JammedReport jr2{waiter,owner};
  jr2.add_monitor("java/lang/Object");
  jr2.add_monitor("SomeOtherClass");

  std::unordered_map<std::string, JammedReport> m1;
  std::unordered_map<std::string, JammedReport> m2;

  m1[jr.key()] = jr;
  m1[jr2.key()] = jr2;
  REQUIRE(m1 != m2);
  m2[jr.key()] = jr;
  m2[jr2.key()] = jr2;
  REQUIRE(m1 == m2);
  
}


TEST_CASE("jammedRpeort: operator >>")
{
  StackTrace owner = frenchroast::testing::build_trace({"mypackage.SomeClass::funcA:(int):void<end-method>1"});
  StackTrace waiter = frenchroast::testing::build_trace({"mypackage.SomeClass::funcB:(int):void<end-method>0"});

  JammedReport jr{waiter,owner};
  jr.add_monitor("LSomeOtherClass");
  jr.add_monitor("Ljava/lang/Object");

  std::stringstream ss;
  ss << jr;
  JammedReport rpt{};
  ss.str() >> rpt;

  REQUIRE(rpt == jr);
}


TEST_CASE("jammedRpeort: operator +=")
{
  StackTrace owner = frenchroast::testing::build_trace({"mypackage.SomeClass::funcA:(int):void<end-method>1"});
  StackTrace waiter = frenchroast::testing::build_trace({"mypackage.SomeClass::funcB:(int):void<end-method>0"});

  JammedReport jam1{waiter,owner};
  jam1.add_monitor("LSomeOtherClass");

  JammedReport jam2{waiter,owner};

  jam2.add_monitor("Ljava/lang/Object");
  jam2.add_monitor("Lmypackage/SomeClass");
  jam1.add_monitor("LSomeOtherClass");

  jam1 += jam2;
  auto mons = jam1.monitors();
  REQUIRE(mons.size() == 3);
  std::sort(mons.begin(), mons.end());
  REQUIRE(mons[0] == "SomeOtherClass");
  REQUIRE(mons[1] == "java.lang.Object");
  REQUIRE(mons[2] == "mypackage.SomeClass");


}
