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

#include "catch.hpp"
#include "JammedViewerModel.h"
#include "JammedReport.h"
#include "qapplication.h"
#include "helper.h"

using namespace frenchroast;
using namespace frenchroast::monitor;



TEST_CASE("JammedViewerModel -  update 2 times")
{
  char name[] = "";
  char* argv[1] = {name};
  int argc =1;
  QApplication* qapp = new QApplication {argc,argv};
 
  QTableWidget* tptr = new QTableWidget{};
  tptr->insertColumn(0);
  tptr->insertColumn(0);
  tptr->insertColumn(0);
  tptr->insertColumn(0);

  StackTrace owner = frenchroast::testing::build_trace({"mypackage.SomeClass::funcA:(int):void<end-method>1"});
  StackTrace waiter = frenchroast::testing::build_trace({"mypackage.SomeClass::funcB:(int):void<end-method>0"});

  JammedReport rpt{waiter,owner};
  rpt.add_monitor("Ljava/lang/Object");  

  std::unordered_map<std::string, int>  jamsRow;
  std::unordered_map<std::string, int>  jamsCount;

  JammedViewerModel model{tptr, jamsRow,jamsCount};
  model.update(rpt);
  REQUIRE(tptr->rowCount() == 1);
  REQUIRE(tptr->item(0,0)->text() == "1");
  REQUIRE(tptr->item(0,1)->text() == "java.lang.Object");
  REQUIRE(tptr->item(0,2)->text().toStdString() == "mypackage.SomeClass::funcB:(int):void");
  REQUIRE(tptr->item(0,3)->text().toStdString() == "mypackage.SomeClass::funcA:(int):void");
  model.update(rpt+=rpt);
  REQUIRE(tptr->rowCount() == 1);
  REQUIRE(tptr->item(0,0)->text() == "2");
  REQUIRE(tptr->item(0,1)->text() == "java.lang.Object");
  REQUIRE(tptr->item(0,2)->text().toStdString() == "mypackage.SomeClass::funcB:(int):void");
  REQUIRE(tptr->item(0,3)->text().toStdString() == "mypackage.SomeClass::funcA:(int):void");
}


TEST_CASE("JammedViewerModel -  update , different traces")
{
  char name[] = "";
  char* argv[1] = {name};
  int argc =1;
  QApplication* qapp = new QApplication {argc,argv};
 
  QTableWidget* tptr = new QTableWidget{};
  tptr->insertColumn(0);
  tptr->insertColumn(0);
  tptr->insertColumn(0);
  tptr->insertColumn(0);

  StackTrace owner = frenchroast::testing::build_trace({"mypackage.SomeClass::funcA:(int):void<end-method>1"});
  StackTrace waiter = frenchroast::testing::build_trace({"mypackage.SomeClass::funcB:(int):void<end-method>0"});

  JammedReport rpt{waiter,owner};
  rpt.add_monitor("Ljava/lang/Object");  

  std::unordered_map<std::string, int>  jamsRow;
  std::unordered_map<std::string, int>  jamsCount;

  JammedViewerModel model{tptr, jamsRow,jamsCount};
  model.update(rpt);
  REQUIRE(tptr->rowCount() == 1);
  REQUIRE(tptr->item(0,0)->text() == "1");
  REQUIRE(tptr->item(0,1)->text() == "java.lang.Object");
  REQUIRE(tptr->item(0,2)->text().toStdString() == "mypackage.SomeClass::funcB:(int):void");
  REQUIRE(tptr->item(0,3)->text().toStdString() == "mypackage.SomeClass::funcA:(int):void");
  
  StackTrace owner2 = frenchroast::testing::build_trace({"mypackage.SomeClass::funcA:(int):void<end-method>1"});
  StackTrace waiter2 = frenchroast::testing::build_trace({"mypackage.SomeClass::funcD:(int):void<end-method>0"});

  JammedReport rpt2{waiter2,owner2};
  rpt2.add_monitor("Ljava/lang/Object");
  rpt2.add_monitor("LSomeClass");  

  model.update(rpt2);
  REQUIRE(tptr->rowCount() == 4);
  REQUIRE(tptr->item(2,0)->text() == "1");
  REQUIRE(tptr->item(2,1)->text() == "SomeClass");
  REQUIRE(tptr->item(3,1)->text() == "java.lang.Object");
  REQUIRE(tptr->item(2,2)->text().toStdString() == "mypackage.SomeClass::funcD:(int):void");
  REQUIRE(tptr->item(2,3)->text().toStdString() == "mypackage.SomeClass::funcA:(int):void");


  StackTrace waiter3 = frenchroast::testing::build_trace({"mypackage.SomeClass::funcD:(int):void<end-method>0",
                                                          "mypackage.OtherClass::otherfunc:(void):void<end-method>0"
                                                         });

  JammedReport rpt3{waiter3,owner};
  rpt3.add_monitor("Ljava/lang/Object");
  model.update(rpt3);
  REQUIRE(tptr->rowCount() == 7);
  REQUIRE(tptr->item(5,0)->text() == "1");
  REQUIRE(tptr->item(5,1)->text() == "java.lang.Object");
  REQUIRE(tptr->item(5,2)->text().toStdString() == "mypackage.SomeClass::funcD:(int):void");
  REQUIRE(tptr->item(6,2)->text().toStdString() == "mypackage.OtherClass::otherfunc:(void):void");
  REQUIRE(tptr->item(5,3)->text().toStdString() == "mypackage.SomeClass::funcA:(int):void");  
}

