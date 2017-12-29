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
  std::vector<std::string> methods;
  methods.push_back("funcA");
  methods.push_back("funcB");
  ClassDetail cd1{"SomeClass", methods};

  s1.update_class_viewer(details);
  
  Session s2= s1;
  REQUIRE((s1 == s2));
}


TEST_CASE("update_class_viewer, once")
{
  Session s1{};
  std::vector<ClassDetail> details;
  std::vector<std::string> methods;
  methods.push_back("funcA");
  methods.push_back("funcB");
  ClassDetail cd1{"SomeClass", methods};

  details.push_back(cd1);
  s1.update_class_viewer(details);
  
  REQUIRE(details == s1.get_class_viewer());
}

TEST_CASE("update_class_viewer, append")
{
  Session s1{};
  std::vector<ClassDetail> details;
  std::vector<std::string> methods;
  methods.push_back("funcA");
  methods.push_back("funcB");
  ClassDetail cd1{"SomeClass", methods};

  details.push_back(cd1);
  s1.update_class_viewer(details);
  ClassDetail cd2{"SomeClassMore", methods};
  details.clear();
  details.push_back(cd2);
  s1.update_class_viewer(details);
  details.clear();
  details.push_back(cd1);
  details.push_back(cd2);
  REQUIRE(details == s1.get_class_viewer());
}


TEST_CASE("operator== and Copy consstructor,  objects are not equal and not empty")
{
  Session s1{};
  std::vector<ClassDetail> details;
  std::vector<std::string> methods;
  methods.push_back("funcA");
  methods.push_back("funcB");
  ClassDetail cd1{"SomeClass", methods};
  details.push_back(cd1);
  s1.update_class_viewer(details);
  
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
  std::vector<std::string> methods;
  methods.push_back("funcA");
  methods.push_back("funcB");
  ClassDetail cd1{"SomeClass", methods};
  ClassDetail cd2{"SomeClassMore", methods};
  details.push_back(cd1);
  details.push_back(cd2);
  Session s1{new PersistorFile{}};
  s1.update_class_viewer(details);
  s1.store("/tmp/session_test.txt");
  Session s2{new PersistorFile{}};
  s2.load("/tmp/session_test.txt");
  REQUIRE(s1 == s2);
}

TEST_CASE("store(fileName) / load (fileName) : empty details")
{
  std::vector<ClassDetail> details;
  Session s1{new PersistorFile{}};
  s1.update_class_viewer(details);
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



TEST_CASE("store(fileName), store()")
{

  std::vector<ClassDetail> details;
  std::vector<std::string> methods;
  methods.push_back("funcA");
  methods.push_back("funcB");
  ClassDetail cd1{"SomeClass", methods};
  ClassDetail cd2{"SomeClassMore", methods};
  details.push_back(cd1);
  details.push_back(cd2);
  Session s1{new PersistorFile{}};
  s1.update_class_viewer(details);
  s1.store("/tmp/session_test.txt");
  details.clear();
  ClassDetail cd3{"SomeClassAgain", methods};
  details.push_back(cd3);
  s1.update_class_viewer(details);
  s1.store();
  Session s2{new PersistorFile{}};
  s2.load("/tmp/session_test.txt");
  REQUIRE(s1 == s2);
}

  

