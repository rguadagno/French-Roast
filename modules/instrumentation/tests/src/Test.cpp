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
#include "FrenchRoast.h"
#include "testing_utils.h"

using   namespace frenchroast;   




TEST_CASE("simple load")
{

  std::string java_class_file = std::string{std::getenv("JAVA_CLASS_DIR")} + "Simple.class";
  MagicComponent mc;
  std::unique_ptr<const frenchroast::BYTE> ptr = fr_test_util::get_class_buf(java_class_file);
  mc.load_from_buffer(ptr.get());
  mc.display(std::cout);
  REQUIRE( mc.size_in_bytes() == 4    );
  REQUIRE( mc.get_number() == 0xCAFEBABE);
  int idx = mc.size_in_bytes();
  IDComponent mv;
  mv.load_from_buffer(ptr.get() + idx);
  REQUIRE( mv.size_in_bytes() == 2    );
  idx += mv.size_in_bytes();
  REQUIRE( mv.get() == 0    );
  IDComponent major;
  major.load_from_buffer(ptr.get() + idx);
  REQUIRE( major.size_in_bytes() == 2    );
  idx += major.size_in_bytes();
  REQUIRE( major.get() >= 50    );
}

TEST_CASE("ConstantPool : add_class : only add a class once")
{
 std::string java_class_file = std::string{std::getenv("JAVA_CLASS_DIR")} + "Simple.class";
  std::unique_ptr<const frenchroast::BYTE> ptr = fr_test_util::get_class_buf(java_class_file);
  ConstantPoolComponent constpool;
  constpool.load_from_buffer(ptr.get() + 8);

  REQUIRE ( constpool.next_index() == 12);    // this is not logicall index, but array
  int idx1  = constpool.add_class("someclass");
  int idx2  = constpool.add_class("someclass");
  REQUIRE ( constpool.next_index() == 14);    // this is not logicall index, but array
  REQUIRE ( idx1 == idx2);    // this is not logicall index, but array
  

}

TEST_CASE("ConstantPool : simple : add_class")
{
  std::string java_class_file = std::string{std::getenv("JAVA_CLASS_DIR")} + "Simple.class";
  std::unique_ptr<const frenchroast::BYTE> ptr = fr_test_util::get_class_buf(java_class_file);
  ConstantPoolComponent constpool;
  constpool.load_from_buffer(ptr.get() + 8);

  REQUIRE ( constpool.next_index() == 12);    // this is not logicall index, but array
  REQUIRE ( constpool.get_name_index("java/lang/Object") == 12);
  REQUIRE ( constpool.get_name_index("someclass") == 0);
  int idx  = constpool.add_class("someclass");
  REQUIRE ( constpool.get_name_index("someclass") == 13);
  REQUIRE ( constpool.get_name_index("13") == 14);
  idx = constpool.add_method_ref_index("someclass.ff:()V");
  REQUIRE ( constpool.get_name_index("ff") == 15);
  //
  //  1. add name ff so that should be 15
  //  2. add a nameAndType 15.5   that should be 16
  //  3. add a method ref  13.16  that should be 17
  //
  
  
  REQUIRE ( idx == 17);
  REQUIRE ( constpool.get_name_index("()V") == 5 );
}

TEST_CASE("ConstantPool : simple")
{
  std::string java_class_file = std::string{std::getenv("JAVA_CLASS_DIR")} + "Simple.class";
  std::unique_ptr<const frenchroast::BYTE> ptr = fr_test_util::get_class_buf(java_class_file);
  ConstantPoolComponent constpool;

  constpool.load_from_buffer(ptr.get() + 8);
  REQUIRE ( constpool.next_index() == 12 );
  REQUIRE ( constpool.size_in_bytes() == 125 );
  REQUIRE ( constpool.add_method_ref_index("java/lang/Object.<init>:()V") == 13 );  

  REQUIRE_THROWS_AS( constpool.add_method_ref_index("<init>:()V") , std::invalid_argument );
  REQUIRE_THROWS_AS( constpool.add_method_ref_index("somepackage/someClasssomefunc()V") , std::invalid_argument );
  constpool.add_class("somepackage/someClass");

  int nextindex = constpool.next_index();
  std::cout << nextindex << std::endl;
  // adds 2 the name somefunc then then methodref so + 3
  REQUIRE( constpool.add_method_ref_index("somepackage/someClass.somefunc:()V") == nextindex + 3); 

  constpool.reset();
  REQUIRE ( constpool.get_method_ref_index("java/lang/Object.<init>:()V") == 0);
  REQUIRE ( constpool.next_index() == 0);
  REQUIRE ( constpool.size_in_bytes() == 2);

  REQUIRE ( constpool.get_name_index("somename") == 0 );
  int idx = constpool.get_name_index("somename");
  int nidx = constpool.add_name("somename");
  REQUIRE ( nidx == idx + 1 );
  REQUIRE ( constpool.get_name_index("somename") == nidx );
 }




TEST_CASE ("to_int") {

  frenchroast::BYTE data[2];
  memset(data,0,2);
  REQUIRE ( to_int(data,2) == 0);

  data[1] = 200;
  REQUIRE ( to_int(data,2) == 200);

  memset(data,0,2);
  data[0]=1;
  data[1]=244;
  REQUIRE ( to_int(data,2) == 500);
}
