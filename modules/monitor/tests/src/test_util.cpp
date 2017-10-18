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

#include "catch.hpp"
#include "MonitorUtil.h"


using namespace frenchroast::monitor;

TEST_CASE("ntoa: 0 pad")
{
  REQUIRE(ntoa(25,0) == "25");
}

TEST_CASE("ntoa: no pad required")
{
  REQUIRE(ntoa(25,2) == "25");
}

TEST_CASE("ntoa: 1 char pad required")
{
  REQUIRE(ntoa(25,3) == "025");
}

TEST_CASE("pad: no pad required")
{

  std::string str{"tester"};
  pad(str,6);
  REQUIRE(str == "tester");
}

TEST_CASE("pad: 1 char pad required, default char  space")
{

  std::string str{"tester"};
  pad(str,7);
  REQUIRE(str == "tester ");
}

TEST_CASE("pad: 1 char pad required, char =x")
{

  std::string str{"tester"};
  pad(str,7, 'x');
  REQUIRE(str == "testerx");
}


TEST_CASE("pad_front:")
{
  std::string str{"100.25"};
  REQUIRE( pad_front(str,6, ".") == "   100.25");
}


TEST_CASE("format millis: full, zero ")
{
  REQUIRE(format_millis(0) == "0:00:00:000");
}

TEST_CASE("format millis: full, 1.5 seconds ")
{
  REQUIRE(format_millis(1500) == "0:00:01:500");
}

TEST_CASE("format millis: full, 1 minuteand 1 micro second")
{
  REQUIRE(format_millis(60*1000+1) == "0:01:00:001");
}

TEST_CASE("format millis: minutes, 1 minuteand 1 micro second")
{
  REQUIRE(format_millis(60*1000+1, FORMAT_MINUTES) == "01");
}

TEST_CASE("parse_type_tokens: empty")
{
  REQUIRE(parse_type_tokens("").size() == 0);
}

TEST_CASE("parse_type_tokens: 1 int")
{
  std::vector<std::string> rv = parse_type_tokens("I");
  REQUIRE(rv.size() == 1);
  REQUIRE(rv[0] == "int");
}

TEST_CASE("parse_type_tokens: 1 bool")
{
  std::vector<std::string> rv = parse_type_tokens("Z");
  REQUIRE(rv.size() == 1);
  REQUIRE(rv[0] == "bool");
}

TEST_CASE("parse_type_tokens: void")
{
  std::vector<std::string> rv = parse_type_tokens("V");
  REQUIRE(rv.size() == 1);
  REQUIRE(rv[0] == "void");
}

TEST_CASE("parse_type_tokens: long")
{
  std::vector<std::string> rv = parse_type_tokens("J");
  REQUIRE(rv.size() == 1);
  REQUIRE(rv[0] == "long");
}

TEST_CASE("parse_type_tokens: byte")
{
  std::vector<std::string> rv = parse_type_tokens("B");
  REQUIRE(rv.size() == 1);
  REQUIRE(rv[0] == "byte");
}

TEST_CASE("parse_type_tokens: char")
{
  std::vector<std::string> rv = parse_type_tokens("C");
  REQUIRE(rv.size() == 1);
  REQUIRE(rv[0] == "char");
}

TEST_CASE("parse_type_tokens: double")
{
  std::vector<std::string> rv = parse_type_tokens("D");
  REQUIRE(rv.size() == 1);
  REQUIRE(rv[0] == "double");
}

TEST_CASE("parse_type_tokens: float")
{
  std::vector<std::string> rv = parse_type_tokens("F");
  REQUIRE(rv.size() == 1);
  REQUIRE(rv[0] == "float");
}

TEST_CASE("parse_type_tokens: short")
{
  std::vector<std::string> rv = parse_type_tokens("S");
  REQUIRE(rv.size() == 1);
  REQUIRE(rv[0] == "short");
}

TEST_CASE("parse_type_tokens: some object")
{
  std::vector<std::string> rv = parse_type_tokens("Lcom/fr/MyClass;");
  REQUIRE(rv.size() == 1);
  REQUIRE(rv[0] == "com/fr/MyClass");
}




TEST_CASE("parse_type_tokens: 1 int 1 bool")
{
  std::vector<std::string> rv = parse_type_tokens("IZ");
  REQUIRE(rv.size() == 2);
  REQUIRE(rv[0] == "int");
  REQUIRE(rv[1] == "bool");
}






