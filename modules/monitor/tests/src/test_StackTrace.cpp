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
#include "StackTrace.h"
#include "helper.h"

using namespace frenchroast::monitor;


TEST_CASE("StackTrace(thread name) ")
{
  StackTrace st{"some thread"};
  REQUIRE(st.thread_name() == "some thread");
}


TEST_CASE("StackTrace, addFrame, descriptor_frames(), monitor = true ")
{
  StackTrace st = frenchroast::testing::build_trace({"mypackage.SomeClass::funcA:(int):void<end-method>1"});
  REQUIRE(st.monitor_frames().size() == 1);
  REQUIRE(st.monitor_frames()[0] == 1);
  std::vector<StackFrame> frames = st.descriptor_frames();
  REQUIRE(frames.size() == 1);
}

TEST_CASE("StackTrace, addFrame, descriptor_frames(), monitor = false ")
{
  StackFrame sf{};
  std::string line = "mypackage.SomeClass::funcA:(int):void<end-method>0";
  line >> sf;
  StackTrace st{};
  st.addFrame(sf);
  REQUIRE(st.monitor_frames().size() == 1);
  REQUIRE(st.monitor_frames()[0] == 0);
  std::vector<StackFrame> frames = st.descriptor_frames();
  REQUIRE(frames.size() == 1);
}


TEST_CASE("StackTrace, key() ")
{
  StackFrame sf{};
  std::string line = "mypackage.SomeClass::funcA:(int):void<end-method>1";
  line >> sf;
  StackTrace st{};
  st.addFrame(sf);
  REQUIRE(st.key() == "mypackage.SomeClass::funcA:(int):void");
}



TEST_CASE("StackTrace, update_monitors() - no update required ")
{
  StackFrame sf{};
  std::string line = "mypackage.SomeClass::funcA:(int):void<end-method>0";
  line >> sf;
  StackTrace st{};
  st.addFrame(sf);
  REQUIRE(st.monitor_frames().size() == 1);
  REQUIRE(st.monitor_frames()[0] == 0);
  line = "mypackage.SomeClass::funcA:(int):void<end-method>0";
  StackFrame sf2{};
  line >> sf2;
  StackTrace st2{};
  st2.addFrame(sf2);
  REQUIRE(st.update_monitors(st2) == false);
  REQUIRE(st.monitor_frames().size() == 1);
  REQUIRE(st.monitor_frames()[0] == 0);
}

TEST_CASE("StackTrace, update_monitors() - update required ")
{
  StackFrame sf{};
  std::string line = "mypackage.SomeClass::funcA:(int):void<end-method>0";
  line >> sf;
  StackTrace st{};
  st.addFrame(sf);
  REQUIRE(st.monitor_frames().size() == 1);
  REQUIRE(st.monitor_frames()[0] == 0);
  line = "mypackage.SomeClass::funcA:(int):void<end-method>1";
  StackFrame sf2{};
  line >> sf2;
  StackTrace st2{};
  st2.addFrame(sf2);
  REQUIRE(st.update_monitors(st2));
  REQUIRE(st.monitor_frames().size() == 1);
  REQUIRE(st.monitor_frames()[0] == 1);
}

TEST_CASE("StackTrace, update_monitors() - update required - subset frame ")
{
  StackFrame sf{};
  std::string line = "mypackage.SomeClass::funcA:(int):void<end-method>0";
  std::string line2 = "mypackage.SomeClass::funcB:(int):void<end-method>0";
  line >> sf;
  StackTrace st{};
  st.addFrame(sf);
  line2 >> sf;
  st.addFrame(sf);
  REQUIRE(st.descriptor_frames().size() == 2);
  REQUIRE(st.monitor_frames().size() == 2);
  REQUIRE(st.monitor_frames()[0] == 0);
  REQUIRE(st.monitor_frames()[1] == 0);
  line = "mypackage.SomeClass::funcB:(int):void<end-method>1";
  StackFrame sf2{};
  line >> sf2;
  StackTrace st2{};
  st2.addFrame(sf2);
  REQUIRE(st2.monitor_frames().size() == 1);
  REQUIRE(st2.monitor_frames()[0] == 1);
  REQUIRE(st.update_monitors(st2));
  REQUIRE(st.monitor_frames().size() == 2);
  REQUIRE(st.monitor_frames()[0] == 0);
  REQUIRE(st.monitor_frames()[1] == 1);
}

TEST_CASE("StackTrace, update_monitors() - no update required sub set")
{
  StackFrame sf{};
  std::string line = "mypackage.SomeClass::funcA:(int):void<end-method>0";
  std::string line2 = "mypackage.SomeClass::funcB:(int):void<end-method>0";
  line >> sf;
  StackTrace st{};
  st.addFrame(sf);
  line2 >> sf;
  st.addFrame(sf);

  REQUIRE(st.descriptor_frames().size() == 2);
  REQUIRE(st.monitor_frames().size() == 2);
  REQUIRE(st.monitor_frames()[0] == 0);
  REQUIRE(st.monitor_frames()[1] == 0);
  line = "mypackage.SomeClass::funcB:(int):void<end-method>0";
  StackFrame sf2{};
  line >> sf2;
  StackTrace st2{};
  st2.addFrame(sf2);
  REQUIRE(st2.monitor_frames().size() == 1);
  REQUIRE(st2.monitor_frames()[0] == 0);
  REQUIRE(st.update_monitors(st2) == false);
  REQUIRE(st.monitor_frames().size() == 2);
  REQUIRE(st.monitor_frames()[0] == 0);
  REQUIRE(st.monitor_frames()[1] == 0);
}


TEST_CASE("operator ==")
{
  StackTrace s1 = frenchroast::testing::build_trace({"mypackage.SomeClass::funcA:(int):void<end-method>1"});
  StackTrace s2 = frenchroast::testing::build_trace({"mypackage.SomeClass::funcA:(int):void<end-method>1"});
  REQUIRE(s1 == s2);
}

TEST_CASE("StackTrace copy constructor")
{
  StackTrace s1 = frenchroast::testing::build_trace({"mypackage.SomeClass::funcA:(int):void<end-method>1"});
  StackTrace s2 = s1;
  REQUIRE(s1 == s2);
}



TEST_CASE("StackTrace operator <<")
{
  StackTrace s1 = frenchroast::testing::build_trace({"mypackage.SomeClass::funcA:(int):void<end-method>1",
                                                     "mypackage.SomeClass::funcB:(int):void<end-method>1"});
  std::stringstream ss;
  ss << s1;
  REQUIRE(ss.str() == "<end-thread-name>1<end-monitor>1<end-monitor><end-monitors>mypackage.SomeClass::funcA:(int):void<end-method>1<end-frame>mypackage.SomeClass::funcB:(int):void<end-method>1<end-frame><end-trace>");
  
}

