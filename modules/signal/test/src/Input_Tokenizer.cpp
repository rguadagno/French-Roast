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
//    along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
//

#include <stdexcept>
#include "catch.hpp"
#include "fr_signals.h"
#include <iostream>



using namespace frenchroast::signal;

TEST_CASE("load good descriptor, bad point")
{
  frenchroast::signal::Signals sigs;
  REQUIRE_THROWS_WITH( sigs.load("mypackage.MyClass::funcA:(int):void <xENTER>"), "signal point bad/missing: <xENTER>");
}

TEST_CASE("load good descriptor, too many points")
{
  frenchroast::signal::Signals sigs;
  REQUIRE_THROWS_WITH( sigs.load("mypackage.MyClass::funcA:(int):void <ENTER><ENTER>"), "signal point bad/missing: <ENTER><ENTER>");
}

TEST_CASE("load good descriptor, missing point")
{
  frenchroast::signal::Signals sigs;
  REQUIRE_THROWS_WITH( sigs.load("mypackage.MyClass::funcA:(int):void"), "signal point bad/missing: ");
}

TEST_CASE("load good descriptor <EXIT>")
{
  frenchroast::signal::Signals sigs;
  sigs.load("mypackage.MyClass::funcA:(int):void <EXIT>");
  
  REQUIRE( sigs.classes().size() == 1);

}

TEST_CASE("load good descriptor <ENTER>")
{
  frenchroast::signal::Signals sigs;
  sigs.load("mypackage.MyClass::funcA:(int):void <ENTER>");
  
  REQUIRE( sigs.classes().size() == 1);
  REQUIRE( sigs["mypackage/MyClass"].size() == 1);
  REQUIRE( sigs["mypackage/MyClass"][0].method_name() == "funcA:(I)V");
  REQUIRE( sigs["mypackage/MyClass"][0].artifacts());
  REQUIRE( sigs["mypackage/MyClass"][0].all() == false);
  REQUIRE( (sigs["mypackage/MyClass"][0].flags() &= Signals::METHOD_ENTER) == true);
  REQUIRE( sigs.get_marker_fields("mypackage/MyClass", "funcA:(I)V").size() == 0);
}

TEST_CASE("load good descriptor <ENTER> and 2 fields")
{
  frenchroast::signal::Signals sigs;
  sigs.load("mypackage.MyClass::funcA:(int):void <ENTER> [total] [pickle]");
  
  REQUIRE( sigs.classes().size() == 1);
  REQUIRE( sigs["mypackage/MyClass"].size() == 1);
  REQUIRE( sigs["mypackage/MyClass"][0].method_name() == "funcA:(I)V");
  REQUIRE( sigs["mypackage/MyClass"][0].artifacts());
  REQUIRE( sigs["mypackage/MyClass"][0].all() == false);
  REQUIRE( (sigs["mypackage/MyClass"][0].flags() &= Signals::METHOD_ENTER) == true);
  REQUIRE( sigs.get_marker_fields("mypackage.MyClass", "funcA:(int):void").size() == 2);
  REQUIRE( sigs.get_marker_fields("mypackage.MyClass", "funcA:(int):void")[0] == "total");
  REQUIRE( sigs.get_marker_fields("mypackage.MyClass", "funcA:(int):void")[1] == "pickle");
  
}

TEST_CASE("load good descriptor <ENTER> , all methods")
{
  frenchroast::signal::Signals sigs;
  sigs.load("mypackage.MyClass::* <ENTER> ");
  
  REQUIRE( sigs.classes().size() == 1);
  REQUIRE( sigs["mypackage/MyClass"].size() == 1);
  REQUIRE( sigs["mypackage/MyClass"][0].method_name() == "*");
  REQUIRE( sigs["mypackage/MyClass"][0].artifacts());
  REQUIRE( sigs["mypackage/MyClass"][0].all());
  REQUIRE( (sigs["mypackage/MyClass"][0].flags() &= Signals::METHOD_ENTER) == true);
}






TEST_CASE("load good descriptor <ENTER> and bad fields")
{
  frenchroast::signal::Signals sigs;
  
  REQUIRE_THROWS_AS( sigs.load("mypackage.MyClass::funcA:(int):void <ENTER> [] [pickle]"), std::invalid_argument);
  
}



TEST_CASE("empty string")
{
  frenchroast::signal::Signals sigs;
  sigs.load("");
  REQUIRE( sigs.classes().size() == 0);
  sigs.load(" ");
  REQUIRE( sigs.classes().size() == 0);
}

TEST_CASE("comment line")
{
  frenchroast::signal::Signals sigs;
  sigs.load("#  mypackage.MyClass::funcA:(int):void <ENTER>");
  REQUIRE( sigs.classes().size() == 0);
}

TEST_CASE("junk line")
{
  frenchroast::signal::Signals sigs;
  REQUIRE_THROWS_AS( sigs.load("junk.junk"), std::invalid_argument);
}


TEST_CASE("load good descriptor <ENTER> , turn off artifacts <ARTIFACTS:OFF>")
{
  frenchroast::signal::Signals sigs;
  sigs.load("mypackage.MyClass::funcA:(int):void <ENTER> <ARTIFACTS:OFF>");
  
  REQUIRE( sigs.classes().size() == 1);
  REQUIRE( sigs["mypackage/MyClass"].size() == 1);
  REQUIRE( sigs["mypackage/MyClass"][0].method_name() == "funcA:(I)V");
  REQUIRE( sigs["mypackage/MyClass"][0].artifacts() == false);
  REQUIRE( sigs["mypackage/MyClass"][0].all() == false);
  REQUIRE( (sigs["mypackage/MyClass"][0].flags() &= Signals::METHOD_ENTER) == true);
}


TEST_CASE("Signals: is_signal_class")
{
  frenchroast::signal::Signals sigs;
  sigs.load("mypackage.MyClass::funcA:(int):void <ENTER> <ARTIFACTS:OFF>");
  REQUIRE(sigs.is_signal_class("mypackage/MyClass"));
}
