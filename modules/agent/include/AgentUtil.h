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

#ifndef AGENTUTIL_H
#define AGENTUTIL_H

#include <vector>
#include "jvmti.h"


enum ARG_TYPE {INT_TYPE, STRING_TYPE, ARRAY_TYPE};

std::vector<ARG_TYPE> typeTokenizer(const std::string& line);
bool format_stack_trace(jvmtiEnv* env, jthread&, std::string&);

class ErrorHandler {
 public:
  static bool check_jvmti_error(jvmtiError error, const std::string& msg);
};


bool get_class_name(jvmtiEnv*, jclass, std::string&);
bool get_thread_name(JNIEnv*,  jvmtiEnv* env, jthread, std::string&);

void delete_refs(JNIEnv* jni_env,jthread*, jint count);
#endif
