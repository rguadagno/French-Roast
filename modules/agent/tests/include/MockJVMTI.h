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

#ifndef MOCKJVMTI_H
#define MOCKJVMTI_H

#include <vector>
#include "jvmti.h"


class MockJVMTI {
  jvmtiEnv _env;
  jvmtiInterface_1_ _mymock;

public:
  jvmtiEnv* env;
  static std::vector<unsigned char*> _buffers;

  MockJVMTI();
  ~MockJVMTI();
  
};

#endif
