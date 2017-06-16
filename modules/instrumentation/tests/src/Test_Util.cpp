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

#include <stdexcept>
#include <bitset>
#include <iostream>
#include <fstream>
#include "catch.hpp"
#include "Util.h"
#include "ClassFileComponent.h"



using   namespace frenchroast;   


TEST_CASE ("write_bytes") {

  BYTE b1 = 0;
  write_bytes(&b1, 10, 1);
  REQUIRE( b1 == 10);

  BYTE b2[2];
  write_bytes(b2, 600, 2); 
  REQUIRE(b2[0] ==  2);      
  REQUIRE(b2[1] ==  88);     

  BYTE b3[2];
  short ss = 600;
  write_big_e_bytes(b3, &ss);
  REQUIRE(b3[0] ==  2);
  REQUIRE(b3[1] ==  88);

}

  TEST_CASE ("split") {

  REQUIRE( split("hello::there", "::").size() == 2 );
  REQUIRE( split("hello::there", "::")[0] == "hello" );
  REQUIRE( split("hello::there", "::")[1] == "there" );
  REQUIRE( split("hello::there::ok", "::")[2] == "ok" );

  
  REQUIRE( split("hello:::there", ":::")[0] == "hello" );
  REQUIRE( split("hello:::there", ":::")[1] == "there" );
  REQUIRE( split("hello:::there:::ok", ":::")[2] == "ok" );


  REQUIRE( split("hello::there~func", "::")[1] == "there~func" );
  REQUIRE( split("hello::there:func", "::")[1] == "there:func" );

  REQUIRE( split("hello,there,func")[0] == "hello" );
  REQUIRE( split("hello,there,func")[1] == "there" );
  REQUIRE( split("hello,there,func")[2] == "func" );

  REQUIRE( split("[hello][there]", "][")[0] == "[hello" );
  REQUIRE( split("[hello][there]", "][")[1] == "there]" );

  
}

