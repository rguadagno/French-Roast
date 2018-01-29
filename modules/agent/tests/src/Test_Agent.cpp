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
#include "fr_signals.h"


std::string get_package_class_filename()
{
#ifdef CONNECTOR_UNIX
  return "../data/Package.class";
#else
  return "..\\data\\Package.class";  
#endif
}

std::string get_SomeClass_filename()
{
#ifdef CONNECTOR_UNIX
  return "../data/SomeClass.class";
#else
  return "..\\data\\SomeClass.class";  
#endif
}


std::unique_ptr<const unsigned char> get_package_class()
{
  return fr_test_util::get_class_buf(get_package_class_filename());
}

std::unique_ptr<const unsigned char> get_SomeClass_class()
{
  return fr_test_util::get_class_buf(get_SomeClass_filename());
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

TEST_CASE("add_hooks")
{
  MockJVMTI jvmtimock;

  class FRMock {
    
  public:
    std::string _callFrom;
    std::string _callTo;
    void add_method_call(const std::string& callFrom, const std::string& callTo, std::bitset<4> flags)
    {
      _callFrom = callFrom;
      _callTo = callTo;
    }
    
    std::vector<std::string> get_method_descriptors()
    {
      std::vector<std::string> rv;
      rv.push_back("funcA:(I):V");
      return rv;
    }
    int size_in_bytes() { return 100; }
    void load_to_buffer(unsigned char* buf) { }
    
    
  };
  
  FRMock mock;
  frenchroast::signal::Signals sigs;
  std::unordered_map<std::string, bool>      artifacts;  
  sigs.load("frpackage.SomeClass::funcA:(int):void <ENTER>");
  jint newlen;
  unsigned char* newbuf;
  add_hooks(mock, sigs, artifacts, "frpackage/SomeClass", jvmtimock.env, &newlen, &newbuf);
  REQUIRE(mock._callFrom == "funcA:(I)V");
  REQUIRE(mock._callTo == "java/lang/Package.thook:(Ljava/lang/Object;)V");

  sigs.load("frpackage.AnotherClass::funcA:(int):void <TIMER>");
  add_hooks(mock, sigs, artifacts, "frpackage/AnotherClass", jvmtimock.env, &newlen, &newbuf);
  REQUIRE(mock._callFrom == "funcA:(I)V");
  REQUIRE(mock._callTo == "java/lang/Package.timerhook:(JLjava/lang/String;Ljava/lang/String;)V");
  
}


TEST_CASE("remove_hooks")
{
  MockJVMTI jvmtimock;
  frenchroast::OpCode::load_from_file( std::getenv("OPCODE_FILE"));
  
  frenchroast::OpCode                        opcodes;
  frenchroast::FrenchRoast                   fr{opcodes};
  fr.load_from_buffer(get_SomeClass_class().get());
  int size = fr.size_in_bytes();
  REQUIRE(size == 256);
  frenchroast::signal::Signals sigs;
  std::unordered_map<std::string, bool>      artifacts;  
  sigs.load("frpackage.SomeClass::func1:(int):void <ENTER>");
  jint newlen;
  unsigned char* newbuf;
  add_hooks(fr, sigs, artifacts, "frpackage/SomeClass", jvmtimock.env, &newlen, &newbuf);
  REQUIRE(fr.size_in_bytes() > size);
  jint restored_len;
  unsigned char* restored_buf;
  remove_hooks(get_SomeClass_class().get(), size, jvmtimock.env, &restored_len, &restored_buf);
  REQUIRE(size == restored_len);
  REQUIRE(memcmp(get_SomeClass_class().get(), restored_buf, size) == 0);

  
}
