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
#include "Session.h"
#include "PersistorFile.h"


using namespace frenchroast::monitor;
using namespace frenchroast::session;


TEST_CASE("operator== and Copy consstructor,  objects are equal and empty")
{
  Session s1{};
  Session s2= s1;
  REQUIRE((s1 == s2));
}

TEST_CASE("operator== and Copy consstructor,  objects are equal and not empty")
{
  Session s1{};
  std::vector<ClassDetail> details;
  "SomeClass<end-name>funcA:(int):void<end-method><end-item>" >> details;
  s1.update(details);
  
  Session s2= s1;
  REQUIRE((s1 == s2));
}


TEST_CASE("update_class_viewer, once")
{
  Session s1{};
  std::vector<ClassDetail> details;
  "SomeClass<end-name>funcA:(int):void<end-method><end-item>" >> details;
  s1.update(details);
  REQUIRE(details == s1.get_loaded_classes());
}


TEST_CASE("update_class_viewer, append")
{
  Session s1{};
  std::vector<ClassDetail> details;
  "SomeClass<end-name>funcA:(int):void<end-method>funcB:(int):void<end-method><end-item>" >> details;
  s1.update(details);
  details.clear();
  "SomeClassMore<end-name>funcA:(int):void<end-method>funcB:(int):void<end-method><end-item>" >> details;
  s1.update(details);
  details.clear();
  "SomeClass<end-name>funcA:(int):void<end-method>funcB:(int):void<end-method><end-item>SomeClassMore<end-name>funcA:(int):void<end-method>funcB:(int):void<end-method><end-item>" >> details;

  REQUIRE(details == s1.get_loaded_classes());
}


TEST_CASE("operator== and Copy consstructor,  objects are not equal and not empty")
{
  Session s1{};
  std::vector<ClassDetail> details;
  "SomeClass<end-name>funcA:(int):void<end-method><end-item>" >> details;
  s1.update(details);
  Session s2;
  REQUIRE((s1 != s2));
  Session s3 = s1;
  REQUIRE((s1 == s3));
}

TEST_CASE("store() with no Persistor")
{
  Session s1{};
  REQUIRE_THROWS_WITH( s1.store("/tmp/session.txt"), "no Persistor set");
}

TEST_CASE("load() with no Persistor")
{
  Session s1{};
  REQUIRE_THROWS_WITH( s1.load("/tmp/session.txt"), "no Persistor set");
}



TEST_CASE("store(fileName) / load (fileName)")
{
  std::vector<ClassDetail> details;
  "SomeClass<end-name>funcA:(int):void<end-method>funcB:(int):void<end-method><end-item>SomeClassMore<end-name>funcA:(int):void<end-method>funcB:(int):void<end-method><end-item>" >> details;
  Session s1{new PersistorFile{}};
  s1.update(details);
  s1.store("/tmp/session_test.txt");
  Session s2{new PersistorFile{}};
  s2.load("/tmp/session_test.txt");
  REQUIRE(s1 == s2);
}

TEST_CASE("store(fileName) / load (fileName) : empty details")
{
  std::vector<ClassDetail> details;
  Session s1{new PersistorFile{}};
  s1.update(details);
  s1.store("/tmp/session_test.txt");
  Session s2{new PersistorFile{}};
  s2.load("/tmp/session_test.txt");
  REQUIRE(s1 == s2);
}




TEST_CASE("store() no Persistor")
{
  Session s1{};
  REQUIRE_THROWS_WITH( s1.store(), "no storage descriptor available");
}

TEST_CASE("store() with Persistor")
{
  Session s1{new PersistorFile{}};
  REQUIRE_THROWS_WITH( s1.store(), "no storage descriptor available");
}

TEST_CASE("has_descriptor() no")
{
  Session s1{};
  REQUIRE(s1.has_descriptor() == false);
}

TEST_CASE("has_descriptor() yes")
{
  std::vector<ClassDetail> details;
  Session s1{new PersistorFile{}};
  s1.update(details);
  s1.store("/tmp/session_test.txt");
  REQUIRE(s1.has_descriptor());
}



TEST_CASE("store(fileName), store()")
{

  std::vector<ClassDetail> details;
  "SomeClass<end-name>funcA:(int):void<end-method>funcB:(int):void<end-method><end-item>SomeClassMore<end-name>funcA:(int):void<end-method>funcB:(int):void<end-method><end-item>" >> details;
  Session s1{new PersistorFile{}};
  s1.update(details);
  s1.store("/tmp/session_test.txt");
  details.clear();
"SomeClassAgain<end-name>funcA:(int):void<end-method>funcB:(int):void<end-method><end-item>" >> details;
  s1.update(details);
  s1.store();
  Session s2{new PersistorFile{}};
  s2.load("/tmp/session_test.txt");
  REQUIRE(s1 == s2);
}


TEST_CASE("reset()")
{
  std::vector<ClassDetail> details;
  "SomeClass<end-name>funcA:(int):void<end-method>funcB:(int):void<end-method><end-item>SomeClassMore<end-name>funcA:(int):void<end-method>funcB:(int):void<end-method><end-item>" >> details;
  Session s1{};
  s1.update(details);
  Session s2{};
  REQUIRE(s1 != s2);
  s1.reset();
  REQUIRE(s1 == s2);  
}


TEST_CASE("load 2 times in row - make sure not duplicated")
{
  std::vector<ClassDetail> details;
  "SomeClass<end-name>funcA:(int):void<end-method>funcB:(int):void<end-method><end-item>SomeClassMore<end-name>funcA:(int):void<end-method>funcB:(int):void<end-method><end-item>" >> details;
  Session s1{new PersistorFile{}};
  s1.update(details);
  s1.store("/tmp/session_test.txt");

  Session s2{new PersistorFile{}};
  s2.load("/tmp/session_test.txt");
  s2.load("/tmp/session_test.txt");
  
  Session s3{new PersistorFile{}};
  s3.load("/tmp/session_test.txt");

  REQUIRE(s3 == s2);
}

 
