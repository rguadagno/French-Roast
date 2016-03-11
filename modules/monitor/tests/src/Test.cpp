// copyright (c) 2016 Richard Guadagno
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

#define CATCH_CONFIG_MAIN
#include <stdexcept>
#include <bitset>
#include <iostream>
#include <fstream>
#include "catch.hpp"
#include "Hooks.h"


const std::string SOME_EMPTY_FILE{"c:\\temp\\empty_config.txt"};
const std::string SOME_BAD_FILE{"c:\\temp\\bad_config.txt"};
const std::string SOME_GOOD_FILE{"c:\\richg\\French-Roast\\modules\\monitor\\hooks_config.txt"};


TEST_CASE("Hooks : load")
{

  frenchroast::monitoring::Hooks hooks{};
  REQUIRE_THROWS( hooks.load("junk") , std::ifstrem::failure );
  REQUIRE_THROWS( hooks.load(SOME_EMPTY_FILE) , std::invalid_argument );
  REQUIRE_THROWS( hooks.load(SOME_BAD_FILE) , std::invalid_argument );

  hooks.load(SOME_GOOD_FILE);
  REQUIRE( hooks.is_hook_class("juniper/MyStuff") );
  REQUIRE( hooks.get("juniper/MyStuff").size() == 2);

  REQUIRE( hooks.get("juniper/MyStuff")[0].method_name() == "isAna:(Ljava/lang/String;Ljava/lang/String;)Z" );
  REQUIRE( hooks.get("juniper/MyStuff")[1].method_name() == "fib:(IIII)V" );

  

  
  
  
}

