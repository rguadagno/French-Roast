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
#include "ClassDetail.h"


using namespace frenchroast::monitor;


TEST_CASE("operator==,  objects are Equal")
{
  std::vector<std::string> methods;
  methods.push_back("funcA");
  methods.push_back("funcB");
  ClassDetail cd{"SomeClass", methods};
  ClassDetail cd2{"SomeClass", methods};
  REQUIRE((cd == cd2));
}

TEST_CASE("operator!=,  objects are Not Equal")
{
  std::vector<std::string> methods;
  methods.push_back("funcA");
  methods.push_back("funcB");
  ClassDetail cd{"SomeClass", methods};
  methods.push_back("funcC");
  ClassDetail cd2{"SomeClass", methods};
  REQUIRE((cd != cd2));
}



TEST_CASE("operator<<")
{
  std::vector<std::string> methods;
  methods.push_back("funcA");
  methods.push_back("funcB");
  ClassDetail cd{"SomeClass", methods};
  std::stringstream ss;
  ss << cd;
  std::string line = ss.str();
  REQUIRE(line == "SomeClass<end-name>funcA<end-method>funcB<end-method>");
}


TEST_CASE("operator<< vector<ClassDetail>")
{
  std::vector<std::string> methods;
  methods.push_back("funcA");
  methods.push_back("funcB");
  
  std::vector<ClassDetail> classes;
  classes.emplace_back("SomeClass",methods);
  classes.emplace_back("SomeClassMore",methods);
  std::stringstream ss;
  ss << classes;
  std::string line = ss.str();
  REQUIRE(line == "SomeClass<end-name>funcA<end-method>funcB<end-method><end-item>SomeClassMore<end-name>funcA<end-method>funcB<end-method><end-item>");
}


TEST_CASE("operator>> vector<ClassDetail>")
{
  std::vector<std::string> methods;
  methods.push_back("funcA");
  methods.push_back("funcB");
  
  std::vector<ClassDetail> classes;
  classes.emplace_back("SomeClass",methods);
  classes.emplace_back("SomeClassMore",methods);
  std::vector<ClassDetail> classes2;
  "SomeClass<end-name>funcA<end-method>funcB<end-method><end-item>SomeClassMore<end-name>funcA<end-method>funcB<end-method><end-item>" >> classes2;

  REQUIRE(classes == classes2);
  
}

TEST_CASE("operator>>")
{
  std::vector<std::string> methods;
  methods.push_back("funcA");
  methods.push_back("funcB");
  ClassDetail cd{"SomeClass", methods};
  ClassDetail cd2{};
  "SomeClass<end-name>funcA<end-method>funcB<end-method>" >> cd2;
  REQUIRE((cd == cd2));
}


TEST_CASE("operator>> from agent")
{
  std::vector<std::string> methods;
  methods.push_back("funcA:(int):void");
  methods.push_back("funcB:(int):void");
  ClassDetail cd{"mypackage.SomeClass", methods};
  ClassDetail cd2{};
  "<agent>mypackage/SomeClass<end-name>funcA:(I)V<end-method>funcB:(I)V<end-method>" >> cd2;
  REQUIRE((cd == cd2));
}






