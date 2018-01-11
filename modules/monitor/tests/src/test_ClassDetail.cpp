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
  std::vector<Descriptor> methods;
  methods.emplace_back("mypackage.SomeClass;::funcA:(I)V");
  methods.emplace_back("mypackage.SomeClass;::funcB:(I)V");
  ClassDetail cd{"SomeClass", methods};
  ClassDetail cd2{"SomeClass", methods};
  REQUIRE((cd == cd2));
}

TEST_CASE("operator!=,  objects are Not Equal")
{
  std::vector<Descriptor> methods;
  methods.emplace_back("Lmypackage/SomeClass;::funcA:(I)V");
  methods.emplace_back("Lmypackage/SomeClass;::funcB:(I)V");
  ClassDetail cd{"SomeClass", methods};
  methods.emplace_back("Lmypackage/SomeClass;::funcC:(I)V");
  ClassDetail cd2{"SomeClass", methods};
  REQUIRE((cd != cd2));
}



TEST_CASE("operator<<")
{
  std::vector<Descriptor> methods;
  methods.emplace_back("LSomeClass::funcA:(I)V");
  methods.emplace_back("LSomeClass::funcB:(I)V");
 
  ClassDetail cd{"SomeClass", methods};
  std::stringstream ss;
  ss << cd;
  std::string line = ss.str();
  REQUIRE(line == "SomeClass<end-name>SomeClass::funcA:(int):void<end-method>SomeClass::funcB:(int):void<end-method>");
}


TEST_CASE("operator<< vector<ClassDetail>")
{
  std::vector<Descriptor> methods;
  methods.emplace_back("Lmypackage/SomeClass::funcA:(I)V");
  methods.emplace_back("Lmypackage/SomeClass::funcB:(I)V");
  
  std::vector<ClassDetail> classes;
  classes.emplace_back("mypackage/SomeClass",methods);
  classes.emplace_back("mypackage/SomeClassMore",methods);
  std::stringstream ss;
  ss << classes;
  std::string line = ss.str();
  REQUIRE(line == "<loaded-classes><view>mypackage.SomeClass<end-name>mypackage.SomeClass::funcA:(int):void<end-method>mypackage.SomeClass::funcB:(int):void<end-method><end-item>mypackage.SomeClassMore<end-name>mypackage.SomeClass::funcA:(int):void<end-method>mypackage.SomeClass::funcB:(int):void<end-method><end-item>");
}


TEST_CASE("operator>> vector<ClassDetail>")
{
  std::vector<Descriptor> methods;
  methods.emplace_back("Lmypackage/SomeClass::funcA:(I)V");
  methods.emplace_back("Lmypackage/SomeClass::funcB:(I)V");
  
  std::vector<ClassDetail> classes;
  classes.emplace_back("mypackage/SomeClass",methods);
  classes.emplace_back("mypackage/SomeClassMore",methods);
  std::vector<ClassDetail> classes2;
  "mypackage.SomeClass<end-name>mypackage.SomeClass::funcA:(int):void<end-method>mypackage.SomeClass::funcB:(int):void<end-method><end-item>mypackage.SomeClassMore<end-name>mypackage.SomeClass::funcA:(int):void<end-method>mypackage.SomeClass::funcB:(int):void<end-method><end-item>" >> classes2;

  REQUIRE(classes == classes2);

  std::vector<ClassDetail> classes3;
  methods.clear();
  methods.emplace_back("Lmypackage/SomeClass::funcA:(I)V");
  classes3.emplace_back("mypackage/SomeClass",methods);
  std::vector<ClassDetail> classes4;
  "mypackage.SomeClass<end-name>mypackage.SomeClass::funcA:(int):void<end-method><end-item>" >> classes4;

  REQUIRE(classes3 == classes4);
  
}

TEST_CASE("operator>>")
{
  std::vector<Descriptor> methods;
  methods.emplace_back("Lmypackage/SomeClass::funcA:(I)V");
  methods.emplace_back("Lmypackage/SomeClass::funcB:(I)V");
  
  ClassDetail cd{"mypackage/SomeClass", methods};
  ClassDetail cd2{};
 "mypackage.SomeClass<end-name>mypackage.SomeClass::funcA:(int):void<end-method>mypackage.SomeClass::funcB:(int):void<end-method>"  >> cd2;
  REQUIRE((cd == cd2));


  
}





