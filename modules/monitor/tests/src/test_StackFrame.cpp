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
#include "StackFrame.h"


using namespace frenchroast::monitor;


TEST_CASE("operator>> StackFrame")
{
  //  StackFrame sf{};
  //  std::string line = "<agent>mypackage/SomeClass<end-name>funcA:(I)V<end-method>";
  //line >> sf;
  //REQUIRE(sf.get_name() == "mypackage.SomeClass::funcA(int):void");
  
}

/*TEST_CASE("StackFrame(funcA,2)")
{
  StackFrame sf{"funcA",2};
  REQUIRE(sf.get_name() == "funcA");
  REQUIRE(sf.get_monitor_count() == 2);
}
*/


