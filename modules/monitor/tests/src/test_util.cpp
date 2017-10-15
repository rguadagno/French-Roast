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


