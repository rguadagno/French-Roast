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


TEST_CASE("operator>>")
{
  std::vector<std::string> methods;
  methods.push_back("funcA");
  methods.push_back("funcB");
  ClassDetail cd{"SomeClass", methods};
  std::stringstream line;
  line << cd;
  ClassDetail cd2{};
  line.str() >> cd2;
  REQUIRE((cd == cd2));
}






