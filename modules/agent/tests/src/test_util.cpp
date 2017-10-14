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
#include "AgentUtil.h"

TEST_CASE("typeTokenizer: empty string")
{
    REQUIRE_THROWS_AS(typeTokenizer(""),  std::invalid_argument);
}

TEST_CASE("typeTokenizer: no args")
{
  REQUIRE(typeTokenizer("()").size() == 0);
}

TEST_CASE("typeTokenizer: 1 int")
{
  std::vector<ARG_TYPE> rv = typeTokenizer("(I)");
  REQUIRE(rv.size() == 1);
  REQUIRE(rv[0] == INT_TYPE);
}

TEST_CASE("typeTokenizer: 2 ints")
{
  std::vector<ARG_TYPE> rv = typeTokenizer("(II)");
  REQUIRE(rv.size() == 2);
  REQUIRE(rv[0] == INT_TYPE);
  REQUIRE(rv[1] == INT_TYPE);
}

TEST_CASE("typeTokenizer: 1 string")
{
  std::vector<ARG_TYPE> rv = typeTokenizer("(Ljava/Object/String;)");
  REQUIRE(rv.size() == 1);
  REQUIRE(rv[0] == STRING_TYPE);
}

TEST_CASE("typeTokenizer: 2 string")
{
  std::vector<ARG_TYPE> rv = typeTokenizer("(Ljava/Object/String;Ljava/Object/String;)");
  REQUIRE(rv.size() == 2);
  REQUIRE(rv[0] == STRING_TYPE);
  REQUIRE(rv[1] == STRING_TYPE);
}

TEST_CASE("typeTokenizer: 1 int 1 string")
{
  std::vector<ARG_TYPE> rv = typeTokenizer("(ILjava/Object/String;)");
  REQUIRE(rv.size() == 2);
  REQUIRE(rv[0] == INT_TYPE);
  REQUIRE(rv[1] == STRING_TYPE);
}

TEST_CASE("typeTokenizer: 1 string 1 int")
{
  std::vector<ARG_TYPE> rv = typeTokenizer("(Ljava/Object/String;I)");
  REQUIRE(rv.size() == 2);
  REQUIRE(rv[0] == STRING_TYPE);
  REQUIRE(rv[1] == INT_TYPE);
}

TEST_CASE("typeTokenizer: 1 array of int")
{
  std::vector<ARG_TYPE> rv = typeTokenizer("([I)");
  REQUIRE(rv.size() == 1);
  REQUIRE(rv[0] == ARRAY_TYPE);
}

TEST_CASE("typeTokenizer: 1 array of some object")
{
  std::vector<ARG_TYPE> rv = typeTokenizer("([Ljava/Object/String;)");
  REQUIRE(rv.size() == 1);
  REQUIRE(rv[0] == ARRAY_TYPE);
}

TEST_CASE("typeTokenizer: 1 array of some object 1 int")
{
  std::vector<ARG_TYPE> rv = typeTokenizer("([Ljava/Object/String;I)");
  REQUIRE(rv.size() == 2);
  REQUIRE(rv[0] == ARRAY_TYPE);
  REQUIRE(rv[1] == INT_TYPE);
}

TEST_CASE("typeTokenizer: 1 int 1 array of some object")
{
  std::vector<ARG_TYPE> rv = typeTokenizer("(I[Ljava/Object/String;)");
  REQUIRE(rv.size() == 2);
  REQUIRE(rv[0] == INT_TYPE);
  REQUIRE(rv[1] == ARRAY_TYPE);
}


