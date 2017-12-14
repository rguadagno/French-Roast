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

#include <string>
#include "AgentSignalReporting.h"
#include "AgentUtil.h"

std::string get_value(JNIEnv* ptr, jobject obj, FieldInfo& field)
{
  if(field._type == "I") return field._name + ":" + std::to_string(ptr->GetIntField(obj, field._id)) + ";";
  if(field._type == "J") return field._name + ":" + std::to_string(ptr->GetLongField(obj, field._id)) + ";";
  if(field._type == "Ljava/lang/String;") return field._name + ":" +  ptr->GetStringUTFChars(jstring(ptr->GetObjectField(obj, field._id)),0);

  return "none for type = " + field._type;
}


void populate_stack( JNIEnv * jni_env, jvmtiEnv* env, jvmtiFrameInfo* frames, int count, std::vector<DescriptorVO>& rv,
                     std::unordered_map<std::string, bool>& artifacts )
{
  void* cacheid;
  char *methodName;
  char *sig;
  char *generic;
  char* className;
  jclass theclass;

  rv.reserve(count);

  for(int idx = 1; idx < count; idx++) {
    ++frames;
    if(!ErrorHandler::check_jvmti_error(env->GetMethodName(frames->method, &methodName,&sig,&generic), "GetMethodName")) continue;
    if(!ErrorHandler::check_jvmti_error(env->Deallocate((unsigned char *)generic ), "Deallocate")) continue;
    if(!ErrorHandler::check_jvmti_error(env->GetMethodDeclaringClass(frames->method, &theclass), "GetMethodDeclaringClass")) continue;
    if(!get_class_name_fast(env, theclass, &className)) continue;
    jni_env->DeleteLocalRef(theclass);
    rv.emplace_back(className, methodName, sig);
    if(idx == 1 && !artifacts[rv[0].descriptor()]) {
      return;
    }
  }
}
