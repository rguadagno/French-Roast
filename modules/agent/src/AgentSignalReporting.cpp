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
#include <mutex>
#include "AgentSignalReporting.h"
#include "AgentUtil.h"
#include "fr_signals.h"

extern frenchroast::signal::Signals  _hooks;
std::mutex _cache_mutex;
std::unordered_map<std::string, FieldInfo> _reportingFields;

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

void populate_class_fields_info(jvmtiEnv* env, char* classDescriptor, jclass theclass, std::unordered_map<std::string, FieldInfo>& gfieldinfo)
{

  static std::unordered_map<void*, int> _cache;
  
  jint fieldcount=0;
  jfieldID* idPtr;
  env->GetClassFields(theclass, &fieldcount, &idPtr);

  char* fieldname;
  char* sigptr;
  char* genericptr;

   _cache_mutex.lock();
   if(_cache.find(idPtr) != _cache.end()) {
     _cache_mutex.unlock();
  return;
  }
  for(int idx = 0; idx < fieldcount; idx++) {
    
    env->GetFieldName(theclass,  *(idPtr + idx), &fieldname, &sigptr, &genericptr);
    std::string desc = classDescriptor;
    desc.append(">");
    desc.append(fieldname);
    gfieldinfo[desc] = FieldInfo{fieldname, sigptr, *(idPtr + idx)};
    _cache[(idPtr + idx)] = 1;
  }
  _cache_mutex.unlock();
}

void populate_artifacts(JNIEnv * ptr, jvmtiEnv* env,  jvmtiFrameInfo* frames, jobject& obj, std::string& params, std::string& fieldValues, std::vector<DescriptorVO>& stack, jthread& aThread)
{
      int slot = 0;
    jstring jsvalue;
      for(auto& argtype : typeTokenizer(stack[0]._methodSignature)) {
        ++slot;
        switch(argtype) {
        case INT_TYPE:
          jint ivalue;
          if(!ErrorHandler::check_jvmti_error(env->GetLocalInt(aThread, 1,slot, &ivalue), "GetLocalInt")) return;
          params.append(std::to_string(ivalue) + ",");

          break;
        case STRING_TYPE:
          jobject ovalue;
          if(!ErrorHandler::check_jvmti_error(env->GetLocalObject(aThread, 1,slot, &ovalue), "GetLocalObject")) return;
          jsvalue = jstring(ovalue);
          params.append(  std::string(ptr->GetStringUTFChars(jsvalue,0)) + ",");
          ptr->DeleteLocalRef(ovalue);
          break;
        case ARRAY_TYPE:
          params.append( "[],");
          break;
        }
      }
    
      if(params.length() > 1) {
        params.erase(params.length() -1 ,1);
      }
    
      
      if(_hooks.get_marker_fields(stack[0]._classSignature, std::string{stack[0]._methodName} + ":" + stack[0]._methodSignature).size() > 0) {
        jclass theclass;
        if(!ErrorHandler::check_jvmti_error(env->GetMethodDeclaringClass(frames[1].method, &theclass), "GetMethodDeclaringClass")) return;
       
        std::string classStr = std::string{stack[0]._classSignature};
        std::string key;

        for(auto& fieldname : _hooks.get_marker_fields(stack[0]._classSignature, std::string{stack[0]._methodName} + ":" + stack[0]._methodSignature)) {
          key = classStr + ">" + fieldname;
          if(_reportingFields.find(key) == _reportingFields.end()) {
            populate_class_fields_info(env, stack[0]._classSignature, theclass, _reportingFields);          
          }
          fieldValues.append(get_value(ptr, obj, _reportingFields[ key]));       
        }
        
        ptr->DeleteLocalRef(theclass);
      }

}
