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
#include "jvmti.h"
#include "catch.hpp"
#include "OpCode.h"
#include "AgentHooks.h"
#include "FrenchRoast.h"
#include "testing_utils.h"
#include "MockJVMTI.h"



std::string get_package_class_filename()
{
#ifdef CONNECTOR_UNIX
  return "../data/Package.class";
#else
  return "..\\data\\Package.class";  
#endif
}

std::unique_ptr<const unsigned char> get_package_class()
{
  return fr_test_util::get_class_buf(get_package_class_filename());
}



TEST_CASE("update java.lang.Package with thook")
{
  MockJVMTI jvmtimock;

  frenchroast::OpCode::load_from_file( std::getenv("OPCODE_FILE"));
  
  frenchroast::OpCode                        opcodes;
  frenchroast::FrenchRoast                   fr{opcodes};

  jint newlen;
  unsigned char* newbuf;
  add_thook_to_package(fr, get_package_class().get(), jvmtimock.env, &newlen, &newbuf);
  std::vector<std::string> v = fr.get_method_descriptors();
  REQUIRE((std::find(v.begin(), v.end(), "thook:(Ljava/lang/Object;)V") != v.end()));
  REQUIRE((std::find(v.begin(), v.end(), "timerhook:(JLjava/lang/String;Ljava/lang/String;)V") != v.end()));

  fr.load_from_buffer(newbuf);
  std::vector<std::string> vafter = fr.get_method_descriptors();
  REQUIRE((std::find(vafter.begin(), vafter.end(), "thook:(Ljava/lang/Object;)V") != vafter.end()));
  REQUIRE((std::find(vafter.begin(), vafter.end(), "timerhook:(JLjava/lang/String;Ljava/lang/String;)V") != vafter.end()));
  
}
