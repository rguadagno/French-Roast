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

#include "AgentUtil.h"
#include <iostream>

std::vector<ARG_TYPE> typeTokenizer(const std::string& sigStr)
{
  std::vector<ARG_TYPE> rv;

  size_t endidx = sigStr.find(")");
  int slot = 1;

  for(size_t idx = 1; idx < endidx; idx++) {
    if(sigStr[idx] == '[') {
      rv.push_back(ARRAY_TYPE);
      ++idx;
      if(sigStr[idx] == 'L') {
        size_t curr = idx;
        idx = sigStr.find(";", curr);
      }
      else {
        ++idx;
      }
      continue;
    }
    if(sigStr[idx] == 'I') {
      rv.push_back(INT_TYPE);
    }
    if(sigStr[idx] == 'L') {
      size_t curr = idx;
      idx = sigStr.find(";", curr);
      if(sigStr.find("String;", curr) < idx ) {
        rv.push_back(STRING_TYPE);
      }
    }
  }
  return rv;
}


std::string formatStackTrace(jvmtiEnv* env, jvmtiFrameInfo* fptr, int frame_count)
{
  std::string rv;
  for(int fidx = frame_count - 1; fidx >= 0; fidx--) {
    char *methodName;
    char *sig;
    char *class_sig;
    char *generic;

    jvmtiError   err = env->GetMethodName(fptr[fidx].method, &methodName,&sig,&generic);
    jclass theclass;
    env->GetMethodDeclaringClass(fptr[fidx].method, &theclass);
    err = env->GetClassSignature(theclass, &class_sig,&generic);
    std::string classinfo{class_sig};
    rv +=  classinfo.substr(1) + "::" + std::string{methodName} + ":" + std::string{sig} + "#";
  }
  return rv;
}
