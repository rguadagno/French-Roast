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
#include "Signal.h"
#include "helper.h"

using namespace frenchroast::monitor;

TEST_CASE("StackReport ")
{
  Descriptor dsc{"Lmypackage/SomeClass;::funcA:(I)V"};

  StackFrame sf{};
  std::string line = "mypackage.SomeClass::funcA:(int):void<end-method>";
  line >> sf;
  StackTrace st{"t1"};
  st.addFrame(sf);
  "mypackage.SomeClass::funcB:(int):void<end-method>" >> sf;
  st.addFrame(sf);
  StackReport rpt{st};
  REQUIRE(rpt.count() == 1);
  ++rpt;
  REQUIRE(rpt.count() == 2);
  std::stringstream ss;
  ss << rpt;
  REQUIRE(ss.str() == "2<end-count>t1<end-thread-name>0<end-monitor>0<end-monitor><end-monitors>mypackage.SomeClass::funcA:(int):void<end-method>0<end-frame>mypackage.SomeClass::funcB:(int):void<end-method>0<end-frame>");

  StackReport rpt2{};
  "2<end-count>t1<end-thread-name>0<end-monitor>0<end-monitor><end-monitors>mypackage.SomeClass::funcA:(int):void<end-method>0<end-frame>mypackage.SomeClass::funcB:(int):void<end-method>0<end-frame>" >> rpt2;
  REQUIRE(rpt == rpt2);
  
}

TEST_CASE("SignalParams ")
{
  std::vector<std::string> items{"100", "some text"};
  SignalParams params{items};
  REQUIRE(params.size() == 2);

  params += "200";
  REQUIRE(params.size() == 3);
  REQUIRE(params[0] == "100");
  REQUIRE(params[1] == "some text");
  REQUIRE(params[2] == "200");

  std::stringstream ss;
  ss << params;
  REQUIRE(ss.str() == "100<end-param>some text<end-param>200<end-param>");

  SignalParams p2{};
  std::stringstream ss2;
  ss2 << p2;
  REQUIRE(ss2.str() == "");
  
  
  "100<end-param>some text<end-param>200<end-param>" >> p2;
  REQUIRE(params == p2);
}


TEST_CASE("SignalMarkers ")
{
  std::vector<std::string> items{"_total:2000", "_name:jones"};
  SignalMarkers markers{items};
  REQUIRE(markers.size() == 2);

  REQUIRE(markers[0].label == "_total");
  REQUIRE(markers[0].value == "2000");

  markers += {"_cost","600"};
  REQUIRE(markers.size() == 3);
  REQUIRE(markers[2].label == "_cost");
  REQUIRE(markers[2].value == "600");

  std::stringstream ss;
  ss << markers;
  REQUIRE(ss.str() == "_total<end-label>2000<end-mark>_name<end-label>jones<end-mark>_cost<end-label>600<end-mark>");

  SignalMarkers m2{};
  std::stringstream ss2;
  ss2 << m2;
  REQUIRE(ss2.str() == "");
  
   "_total<end-label>2000<end-mark>_name<end-label>jones<end-mark>_cost<end-label>600<end-mark>" >> m2;
  REQUIRE(markers == m2);
}





TEST_CASE("Signal, construction, empty params, empty markers ")
{
  Descriptor dsc{"Lmypackage/SomeClass;::funcA:(I)V"};

  StackFrame sf{};
  std::string line = "mypackage.SomeClass::funcA:(int):void<end-method>";
  line >> sf;
  StackTrace st{"t1"};
  st.addFrame(sf);
  "mypackage.SomeClass::funcB:(int):void<end-method>" >> sf;
  st.addFrame(sf);
  Signal sig{StackReport{st},SignalParams{},SignalMarkers{}};
  REQUIRE(sig.key() == "t1" + dsc.full_name());
  REQUIRE(sig.count() == 1);
  REQUIRE(sig.descriptor() == "mypackage.SomeClass::funcA:(int):void");

  REQUIRE(sig.params().size() == 0);
  REQUIRE(sig.markers().size() == 0);

  std::vector<std::string> items{"100", "some text"};
  SignalParams params{items};
  Signal sig2{StackReport{st},params,SignalMarkers{}};
  REQUIRE(sig2.params().size() == 2);

  std::stringstream ss;
  ss << sig2;
  REQUIRE(ss.str() == "1<end-count>t1<end-thread-name>0<end-monitor>0<end-monitor><end-monitors>mypackage.SomeClass::funcA:(int):void<end-method>0<end-frame>mypackage.SomeClass::funcB:(int):void<end-method>0<end-frame><end-report>100<end-param>some text<end-param><end-params>");


  Signal sig3{};
  "1<end-count>t1<end-thread-name>0<end-monitor>0<end-monitor><end-monitors>mypackage.SomeClass::funcA:(int):void<end-method>0<end-frame>mypackage.SomeClass::funcB:(int):void<end-method>0<end-frame><end-report>100<end-param>some text<end-param><end-params>" >> sig3;
  REQUIRE(sig2 == sig3);
  
}


