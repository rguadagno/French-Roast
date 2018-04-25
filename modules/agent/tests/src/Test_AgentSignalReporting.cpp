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
#include "AgentSignalReporting.h"
#include "MockJNI.h"

#include <iostream>


TEST_CASE("get_value")
{
  using namespace frenchroast::agent;
  
  FieldInfo field_info;
  jobject obj;
  MockJNI jnimock;

  field_info._type = "I";
  field_info._name = "_total";
  REQUIRE(get_value(jnimock.env, obj, field_info) == "_total:100;");
  field_info._type = "J";
  REQUIRE(get_value(jnimock.env, obj, field_info) == "_total:200;");
  field_info._type = "junk";
  REQUIRE(get_value(jnimock.env, obj, field_info) == "none for type = junk");  
}

