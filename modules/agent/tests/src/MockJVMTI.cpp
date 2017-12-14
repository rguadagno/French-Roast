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

#include "MockJVMTI.h"

jvmtiError JNICALL myAllocate(jvmtiEnv* env, jlong size, unsigned char** mem_ptr);



  MockJVMTI::MockJVMTI()
  {
    _mymock.Allocate = &myAllocate;
    _env.functions = &_mymock;
    env = &_env;
  }
  
  MockJVMTI::~MockJVMTI()
  {
    for(auto ptr : _buffers)
      delete ptr;
    _buffers.clear();
  }
  
jvmtiError JNICALL myAllocate(jvmtiEnv* env, jlong size, unsigned char** mem_ptr)
{
  *mem_ptr = reinterpret_cast<unsigned char*>(new char[size]);
  MockJVMTI::_buffers.push_back(*mem_ptr);
  return JVMTI_ERROR_NONE;
}

std::vector<unsigned char*> MockJVMTI::_buffers;
