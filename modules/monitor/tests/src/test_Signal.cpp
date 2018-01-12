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

  "100<end-param>some text<end-param>200<end-param>" >> p2;
  REQUIRE(params == p2);
  
}


TEST_CASE("SignalMarkers ")
{
  std::vector<std::string> items{"_total:2000", "_name:jones"};
  SignalMarkers markers{items};
  REQUIRE(markers.size() == 2);

  //  REQUIRE(markers[0].label == "_total");
  //REQUIRE(markers[0].value == "2000");
}




/*
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


  
  //  Descriptor dsc{"Lmypackage/SomeClass;::funcA:(I)V"};
  //std::stringstream ss;
  //ss << dsc;
  //REQUIRE(ss.str() == "mypackage.SomeClass::funcA:(int):void");

  
}
*/

