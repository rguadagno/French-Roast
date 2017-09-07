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

#include <iostream>
#include <string>
#include "AgentUtil.h"

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


bool format_stack_trace(jvmtiEnv* env, jthread& thread, std::string& trace)
{
  jint frame_count;
  jvmtiFrameInfo frame_info[20];
  if(!ErrorHandler::check_jvmti_error(env->GetStackTrace(thread,0,20, frame_info, &frame_count), "GetStackTrace")) return false;

  for(int fidx = frame_count - 1; fidx >= 0; fidx--) {
    char *methodName;
    char *sig;
    char *generic;

    if(!ErrorHandler::check_jvmti_error(env->GetMethodName(frame_info[fidx].method, &methodName,&sig,&generic), "GetMethodName")) return false;
    jclass theclass;
    env->GetMethodDeclaringClass(frame_info[fidx].method, &theclass);
    
    std::string classinfo;
    if(!get_class_name(env, theclass, classinfo)) return false;
    trace +=  classinfo.substr(1) + "::" + std::string{methodName} + ":" + std::string{sig} + "#";
  }
  return true;;
}


inline bool ErrorHandler::check_jvmti_error(jvmtiError error, const char* msg)
{
  if(error == JVMTI_ERROR_NONE) return true;
  std::cout << "JVMTI ERROR: " << msg << std::endl;
  return false;
}

inline bool get_class_name(jvmtiEnv* env, jclass theclass, std::string& name)
{
  char *class_sig;
  char *generic;
  if(!ErrorHandler::check_jvmti_error(env->GetClassSignature(theclass, &class_sig,&generic), "GetClassSignature")) return false;
  name = std::string{class_sig};
  env->Deallocate(reinterpret_cast<unsigned char*>(class_sig));
  env->Deallocate(reinterpret_cast<unsigned char*>(generic));
  return true;
}

bool get_class_name_fast(jvmtiEnv* env, jclass theclass, char** class_sig)
{
  char *generic;
  if(!ErrorHandler::check_jvmti_error(env->GetClassSignature(theclass, class_sig,&generic), "GetClassSignature")) return false;
  env->Deallocate(reinterpret_cast<unsigned char*>(generic));
  return true;
}


void delete_refs(JNIEnv* jni_env, jthread* tptr, jint count)
{
  for(int idx = 0; idx < count; idx++) {
    jthread thd = *(tptr + idx);
    jni_env->DeleteLocalRef(thd);
  }

}

bool get_thread_name(JNIEnv* jni_env, jvmtiEnv* env, jthread thd, std::string& name)
{
    jvmtiThreadInfo tinfo;
    if(!ErrorHandler::check_jvmti_error(env->GetThreadInfo(thd, &tinfo), "GetThreadInfo")) return false;;
    name = std::string{tinfo.name};
    jthreadGroup tg = tinfo.thread_group;
    jni_env->DeleteLocalRef(tg);
    jobject ctx = tinfo.context_class_loader;
    jni_env->DeleteLocalRef(ctx);
    env->Deallocate((unsigned char *)tinfo.name);
  return true;
}

bool get_thread_name_fast(JNIEnv* jni_env, jvmtiEnv* env, jthread thd, char** name)
{
    jvmtiThreadInfo tinfo;
    if(!ErrorHandler::check_jvmti_error(env->GetThreadInfo(thd, &tinfo), "GetThreadInfo")) return false;;
    *name = tinfo.name;
    jthreadGroup tg = tinfo.thread_group;
    jni_env->DeleteLocalRef(tg);
    jobject ctx = tinfo.context_class_loader;
    jni_env->DeleteLocalRef(ctx);
    
  return true;
}
