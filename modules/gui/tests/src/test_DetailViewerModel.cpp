// copyright (c) 2018 Richard Guadagno
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
#include <unordered_map>
#include "catch.hpp"
#include "DetailViewerModel.h"
#include "StackReport.h"
#include "qapplication.h"

using namespace frenchroast;
using namespace frenchroast::monitor;


TEST_CASE("DetailViewerModel -  stacks update")
{
  char* args[] = {""};
  int argc =1;
  QApplication qapp{argc,args};
  std::unordered_map<std::string, StackReport> stacks;
  std::unordered_map<std::string, QTableWidgetItem*>  items;
  
  QTableWidget* ptr = new QTableWidget{};
  QTableWidget* argptr = new QTableWidget{};
  ptr->insertColumn(0);
  ptr->insertColumn(0);
  
  DetailViewerModel model{ptr,argptr,items};

  model.update_stack_view(stacks);

  StackFrame sf{};
  std::string line = "mypackage.SomeClass::funcA:(int):void<end-method>";
  line >> sf;
  StackTrace st{"t1"};
  st.addFrame(sf);
  "mypackage.SomeClass::funcB:(int):void<end-method>" >> sf;
  st.addFrame(sf);
  StackReport rpt{st};
  stacks[rpt.key()] = rpt;
  model.update_stack_view(stacks);
  REQUIRE(ptr->rowCount()  == 2); 
  REQUIRE(ptr->item(1,1) != nullptr);
  REQUIRE(ptr->item(0,0)->text() == "1");
  REQUIRE(ptr->item(0,1)->text() == "mypackage.SomeClass::funcA:(int):void");
  REQUIRE(ptr->item(1,1)->text() == "mypackage.SomeClass::funcB:(int):void");
}


TEST_CASE("DetailViewerModel: args, initial cols, no instance markers")
{
  char* args[] = {""};
  int argc =1;
  QApplication qapp{argc,args};
  std::unordered_map<std::string, StackReport>        stacks;
  std::unordered_map<std::string, QTableWidgetItem*>  items;
  
  QTableWidget* stackptr = new QTableWidget{};
  QTableWidget* argptr = new QTableWidget{};

  DetailViewerModel model{stackptr,argptr,items};

  model.init_arg_instance_headers({"int"},{});
  REQUIRE(argptr->columnCount()  == 4);
  REQUIRE(argptr->rowCount()  == 0);
  REQUIRE(argptr->rowCount()  == 0);
  REQUIRE(argptr->horizontalHeaderItem(0)->text()  == "invoked");
  REQUIRE(argptr->horizontalHeaderItem(1)->text()  == "(");
  REQUIRE(argptr->horizontalHeaderItem(2)->text()  == "int");
  REQUIRE(argptr->horizontalHeaderItem(3)->text()  == ")"); 
}

TEST_CASE("DetailViewerModel: args, initial cols, instance markers")
{
  char* args[] = {""};
  int argc =1;
  QApplication qapp{argc,args};
  std::unordered_map<std::string, StackReport>        stacks;
  std::unordered_map<std::string, QTableWidgetItem*>  items;
  
  QTableWidget* stackptr = new QTableWidget{};
  QTableWidget* argptr = new QTableWidget{};

  DetailViewerModel model{stackptr,argptr,items};

  model.init_arg_instance_headers({"int"},{"_total"});
  REQUIRE(argptr->columnCount()  == 5);
  REQUIRE(argptr->rowCount()  == 0);
  REQUIRE(argptr->horizontalHeaderItem(0)->text()  == "invoked");
  REQUIRE(argptr->horizontalHeaderItem(1)->text()  == "(");
  REQUIRE(argptr->horizontalHeaderItem(2)->text()  == "int");
  REQUIRE(argptr->horizontalHeaderItem(3)->text()  == ")");
  REQUIRE(argptr->horizontalHeaderItem(4)->text()  == "_total"); 
  
}

TEST_CASE("DetailViewerModel: two args, initial cols, no instance markers")
{
  char* args[] = {""};
  int argc =1;
  QApplication qapp{argc,args};
  std::unordered_map<std::string, StackReport>        stacks;
  std::unordered_map<std::string, QTableWidgetItem*>  items;
  
  QTableWidget* stackptr = new QTableWidget{};
  QTableWidget* argptr = new QTableWidget{};

  DetailViewerModel model{stackptr,argptr,items};

  model.init_arg_instance_headers({"int","int"},{});
  REQUIRE(argptr->columnCount()  == 5);
  REQUIRE(argptr->rowCount()  == 0);
  REQUIRE(argptr->horizontalHeaderItem(0)->text()  == "invoked");
  REQUIRE(argptr->horizontalHeaderItem(1)->text()  == "(");
  REQUIRE(argptr->horizontalHeaderItem(2)->text()  == "int");
  REQUIRE(argptr->horizontalHeaderItem(3)->text()  == "int");
  REQUIRE(argptr->horizontalHeaderItem(4)->text()  == ")");

  
}
