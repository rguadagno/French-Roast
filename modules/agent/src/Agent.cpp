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
//    along with French-Roast.  If not, see <http://www.gnu.org/licenses/>.
//

#include <iostream>
#include <string>
#include <fstream>
#include <mutex>
#include <thread>
#include <atomic>
#include <condition_variable>
#include <unordered_set>
#include "jvmti.h"
#include "Agent.h"
#include "Hooks.h"
#include "FrenchRoast.h"
#include "OpCode.h"
#include "Reporter.h"
#include "Config.h"
#include "CommandListener.h"
#include "CoutTransport.h"
#include "FileTransport.h"
#include "ServerTransport.h"
#include "Listener.h"
#include "AgentUtil.h"
#include "ClassDetail.h"

std::mutex _traffic_mutex;
std::mutex _loading_mutex;
std::mutex _loading_data_mutex;
std::mutex _profiler_mutex;
std::mutex _all_loaded_mutex;
std::mutex _jammed_mutex;

class ClassPtr {
public:
  ClassPtr(jint size) : _size(size) {}
  ClassPtr() {}
  unsigned char* _class_data;
   jint           _size;
};

std::unordered_set<std::string>           _all_loadedClasses;
std::unordered_map<std::string, ClassPtr> _origClass;

frenchroast::network::Connector  _conn;

bool profiler_predicate();

class CommandBridge : public frenchroast::agent::CommandListener, public frenchroast::network::Listener {
  
public:
  std::atomic<int>        _millis{};
  std::atomic<bool>       _runProfile{false};
  std::condition_variable _traffic_cond;
  std::atomic<int>        _load_watch_interval{};
  std::condition_variable _load_watch_cond;
  std::condition_variable _profilerCond;

    void message(const std::string& msg)
    {
      std::vector<std::string> items = frenchroast::split(msg,"~");

      if (items[1] == "stop_watch_traffic") { 
        stop_watch_traffic();
      }
      if (items[1] == "watch_traffic") {
        watch_traffic(atoi(items[2].c_str()));
      }
      if (items[1] == "watch_loading") { 
        watch_loading();
      }
      if (items[1] == "stop_watch_loading") { 
        stop_watch_loading();
      }
      if (items[1] == "turn_on_profiler") { 
        turn_on_profiler();
      }
      if (items[1] == "turn_off_profiler") { 
        turn_off_profiler();
      }
      
    }


  void watch_traffic(const int interval_millis)
  {
    if(!profiler_predicate()) return;
    std::cout << " ** START TRAFFIC **" << std::endl;
    std::lock_guard<std::mutex> lck{_traffic_mutex};
    _millis.store(interval_millis);
    _traffic_cond.notify_one();
  }

  void stop_watch_traffic()
  {
    std::lock_guard<std::mutex> lck{_traffic_mutex};
    std::cout << " ** STOP TRAFFIC **" << std::endl;
    _millis.store(-1);
    _traffic_cond.notify_one();
  }

  void stop_watch_loading()
  {
    std::lock_guard<std::mutex> lck{_loading_mutex};
    _load_watch_interval.store(-1);
    _load_watch_cond.notify_one();

    std::cout << "** STOP WATCH LOADING **" << std::endl;
  }

  void watch_loading()
  {
    if(!profiler_predicate()) return;
    std::lock_guard<std::mutex> lck{_loading_mutex};
    _load_watch_interval.store(5000);
    _load_watch_cond.notify_one();

    std::cout << "** START WATCH LOADING **" << std::endl;
  }

  void turn_on_profiler()
  {
    std::lock_guard<std::mutex> lck{_profiler_mutex};
    _runProfile.store(true);
    _profilerCond.notify_one();
  }

  void turn_off_profiler()
  {
    std::lock_guard<std::mutex> lck{_profiler_mutex};
    _runProfile.store(false);
    _profilerCond.notify_one();
    stop_watch_loading();
    stop_watch_traffic();
  }

  
};




class FieldInfo {
public:
  std::string _name{};
  std::string _type{};
  jfieldID    _id;
  
  FieldInfo(std::string name, std::string typ, jfieldID id) : _name(name), _type(typ), _id(id)
  {
  }
  
  FieldInfo()
  {
  }
  
};

std::vector<frenchroast::monitor::ClassDetail>                  _loadedClasses;
std::unordered_map<std::string, FieldInfo> _reportingFields;
frenchroast::OpCode                        _opcodes;
frenchroast::FrenchRoast                   _fr{_opcodes};
frenchroast::agent::Hooks                  _hooks;
frenchroast::agent::Config                 _config;
frenchroast::agent::Reporter               _rptr;
CommandBridge                              _commandListener;
std::mutex                                 _sig_mutex;
std::mutex                                 _sig_time_mutex;

jvmtiEnv* genv;
JavaVM*   g_java_vm;
JNIEnv*   gxenv;


void JNICALL
ThreadStart(jvmtiEnv *env, JNIEnv* jni_env, jthread thread) {
  jvmtiThreadInfo info;
    env->GetThreadInfo(thread, &info);
    std::cout << "Thred Started: " << info.name << std::endl;
}


JNIEXPORT void JNICALL Java_java_lang_Package_timerhook(JNIEnv * ptr, jclass object, jlong stime, jstring tag, jstring tname)
{
  jvmtiFrameInfo frames[5];
  jint count;
  jvmtiError err;
  jthread aThread;

  genv->GetCurrentThread(&aThread);
  err = genv->GetStackTrace(aThread, 0, 5, frames, &count);
  if (err == JVMTI_ERROR_NONE && count >= 1) {
    char *methodName;
    char *sig;
    char *generic;
    err = genv->GetMethodName(frames[1].method, &methodName,&sig,&generic);
    if (err == JVMTI_ERROR_NONE) {
      std::string methodNameStr{methodName};
      std::string sigStr{sig};

      jclass theclass;
      genv->GetMethodDeclaringClass(frames[1].method, &theclass);
      err = genv->GetClassSignature(theclass, &sig,&generic);
      std::string classinfo{sig};

      _sig_time_mutex.lock();
      _rptr.signal_timer(stime, std::string(ptr->GetStringUTFChars(tag,0)), classinfo +"::" + methodNameStr + ":" +sigStr, std::string(ptr->GetStringUTFChars(tname,0)));
      _sig_time_mutex.unlock();
    }
  }
}


std::string get_value(JNIEnv* ptr, jobject obj, FieldInfo& field)
{
  if(field._type == "I") return field._name + ":" + std::to_string(ptr->GetIntField(obj, field._id)) + ";";
  if(field._type == "J") return field._name + ":" + std::to_string(ptr->GetLongField(obj, field._id)) + ";";
  if(field._type == "Ljava/lang/String;") return field._name + ":" +  ptr->GetStringUTFChars(jstring(ptr->GetObjectField(obj, field._id)),0);

  return "none for type = " + field._type;
}


void populate_class_fields_info(std::string classDescriptor, jclass theclass, std::unordered_map<std::string, FieldInfo>& gfieldinfo)
{

  jint fieldcount=0;
  jfieldID* idPtr;
  genv->GetClassFields(theclass, &fieldcount, &idPtr);

  char* nameptr;
  char* sigptr;
  char* genericptr;


  for(int idx = 0; idx < fieldcount; idx++) {
    genv->GetFieldName(theclass,  *(idPtr + idx), &nameptr, &sigptr, &genericptr);
    std::string fieldName{nameptr};
    gfieldinfo[classDescriptor + ">" + fieldName] = FieldInfo{fieldName, std::string{sigptr}, *(idPtr + idx)};
  }
}



class DescriptorVO {
public:
  std::string _classSignature;
  std::string _methodName;
  std::string _methodSignature;
  DescriptorVO(std::string cSig, std::string mname, std::string mSig) : _classSignature(cSig), _methodName(mname), _methodSignature(mSig)
  {
  }

  std::string descriptor() const
  {
    return _classSignature + "::" + _methodName + ":" + _methodSignature;
  }
};

std::vector<DescriptorVO> populate_stack(  jvmtiFrameInfo* frames, int count)
{
  std::vector<DescriptorVO> rv;
  for(int idx = 1; idx < count; idx++) {
    ++frames;
    char *methodName;
    char *methodSig;
    char *generic;
    genv->GetMethodName(frames->method, &methodName,&methodSig,&generic);
    std::string methodNameStr{methodName};
    std::string sigStr{methodSig};
    jclass theclass;
    char *classSig;
    genv->GetMethodDeclaringClass(frames->method, &theclass);
    genv->GetClassSignature(theclass, &classSig,&generic);
    std::string classinfo{classSig};
     rv.emplace_back(classinfo, methodNameStr, sigStr);
  }
  return rv;
}


JNIEXPORT void JNICALL Java_java_lang_Package_thook (JNIEnv * ptr, jclass klass, jobject obj)
{
  jvmtiFrameInfo frames[10];
  jint count;
  jvmtiError err;
  jthread aThread;

  genv->GetCurrentThread(&aThread);
  jvmtiThreadInfo info;
  genv->GetThreadInfo(aThread, &info);
  
  err = genv->GetStackTrace(aThread, 0, sizeof(frames), frames, &count);
  if (err == JVMTI_ERROR_NONE && count >= 1) {
    std::vector<DescriptorVO> stack = populate_stack(frames, count);
    std::string params = "(";
    int slot = 0;
    jstring jsvalue;
      for(auto& argtype : typeTokenizer(stack[0]._methodSignature)) {
        ++slot;
        switch(argtype) {
        case INT_TYPE:
          jint ivalue;
          genv->GetLocalInt(aThread, 1,slot, &ivalue);
          params.append(std::to_string(ivalue) + ",");
          break;
        case STRING_TYPE:
          jobject ovalue;
          genv->GetLocalObject(aThread, 1,slot, &ovalue);
          jsvalue = jstring(ovalue);
          params.append(  std::string(ptr->GetStringUTFChars(jsvalue,0)) + ",");
          break;
        case ARRAY_TYPE:
          params.append( "[],");
          break;
        }
      }

      if(params.length() > 1) {
        params.erase(params.length() -1 ,1);
      }
      params.append(")");
      jclass theclass;
      genv->GetMethodDeclaringClass(frames[1].method, &theclass);
      std::string fieldValues = "";
      for(auto& fieldname : _hooks.get_marker_fields(stack[0]._classSignature, stack[0]._methodName + ":" + stack[0]._methodSignature)) {
        if(_reportingFields.count(stack[0]._classSignature + ">" + fieldname) == 0) {
          populate_class_fields_info(stack[0]._classSignature, theclass, _reportingFields);          
        }
        fieldValues += get_value(ptr, obj, _reportingFields[stack[0]._classSignature + ">" + fieldname]);       
      }

      std::string stackstr = "";
      for(auto& x : stack) {
        stackstr.append(x.descriptor());
        stackstr.append("%");
        }
      _sig_mutex.lock();
      _rptr.signal(stack[0].descriptor() + "~" + std::string{info.name} + "~" + fieldValues + "~" + params + "~" + stackstr);
      _sig_mutex.unlock();
  }
  }


bool profiler_predicate() 
{
  return _commandListener._runProfile.load();
}


void reload_monitor() 
{
  g_java_vm->AttachCurrentThread((void**)&gxenv,(void*)NULL);
  std::unique_lock<std::mutex> lck{_profiler_mutex};
  _commandListener._profilerCond.wait(lck);
  while(1) {
    _all_loaded_mutex.lock();
    int total = 0;
    for(auto& x : _hooks.classes()) {
      if(_all_loadedClasses.count(x) == 1) {
        ++total;
      }
    }
    jclass* tclasses;      
    genv->Allocate( total * sizeof(jclass), reinterpret_cast<unsigned char**>(&tclasses));
    jclass* ptr = tclasses;
    int idx = 0;
    jclass theclass;
    for(auto& cname : _hooks.classes()) {
      if(_all_loadedClasses.count(cname) == 1) {
        theclass = gxenv->FindClass(cname.c_str()); 
        memcpy(ptr, &theclass, sizeof(jclass));
        ++ptr;
      }
    }
    _all_loaded_mutex.unlock();
    genv->RetransformClasses(total, tclasses);
    _commandListener._profilerCond.wait(lck);

  }
}



bool ok_to_track(const std::string& name)
{
  return name.find("java/") == std::string::npos && name.find("sun/") == std::string::npos;
}

void track_class(const std::string& name)
{

  std::vector<std::string> descriptors;
  for(auto methdesc : _fr.get_method_descriptors()) {
    if(methdesc.find("main") == std::string::npos ) {
      descriptors.push_back(methdesc);
    }
  }
  _loading_data_mutex.lock();
  _loadedClasses.emplace_back(name, descriptors);
  _loading_data_mutex.unlock();
}



void remove_hooks(const std::string& sname, jvmtiEnv *env,jint*& new_class_data_len, unsigned char** new_class_data)
{
  jvmtiError  err =    env->Allocate(_origClass[sname]._size, new_class_data);
  *new_class_data_len = _origClass[sname]._size;
  memcpy(*new_class_data, _origClass[sname]._class_data, _origClass[sname]._size);
}




void add_hooks(const std::string& sname, jvmtiEnv *env,jint*& new_class_data_len, unsigned char** new_class_data)
{
  for (auto& x : _hooks.get(sname)) {
    if (x.line_number() > 0) {
      _fr.add_method_call(x.method_name(), "java/lang/Package.thook:()V", x.line_number());
    }
    else {
      if ((x.flags() & frenchroast::FrenchRoast::METHOD_TIMER) == frenchroast::FrenchRoast::METHOD_TIMER) {
        _fr.add_method_call(x.method_name(), "java/lang/Package.timerhook:(JLjava/lang/String;Ljava/lang/String;)V", x.flags());
      }
      else {
        if(x.all()) {
          for(auto methdesc : _fr.get_method_descriptors()) {
            if(methdesc.find("main") == std::string::npos         ) {
              if( methdesc.find("<init") == std::string::npos ) {
                _fr.add_method_call(methdesc, "java/lang/Package.thook:(Ljava/lang/Object;)V", x.flags());
              }
              else {
                  _fr.add_method_call(methdesc, "java/lang/Package.thook:(Ljava/lang/Object;)V", frenchroast::FrenchRoast::METHOD_EXIT);
              }
            }
          }
        }
        else {
          _fr.add_method_call(x.method_name(), "java/lang/Package.thook:(Ljava/lang/Object;)V", x.flags());
        }
      }
    }
    jint size = _fr.size_in_bytes();
    jvmtiError  err =    env->Allocate(size,new_class_data);
    *new_class_data_len = size;
    _fr.load_to_buffer(*new_class_data);
  }
}


void JNICALL
     ClassFileLoadHook(
     jvmtiEnv *env,
     JNIEnv* jni_env,
     jclass class_being_redefined,
     jobject loader,
     const char* name,
     jobject protection_domain,
     jint class_data_len,
     const unsigned char* class_data,
     jint* new_class_data_len,
     unsigned char** new_class_data) {
  
  std::string sname{name};
  if (sname == "java/lang/Package") {
    _fr.load_from_buffer(class_data);
    
    _fr.add_name_to_pool("thook");
    _fr.add_name_to_pool("(Ljava/lang/Object;)V");
    _fr.add_native_method("thook", "(Ljava/lang/Object;)V");
    
    _fr.add_name_to_pool("timerhook");
    _fr.add_name_to_pool("(JLjava/lang/String;Ljava/lang/String;)V");
    _fr.add_native_method("timerhook", "(JLjava/lang/String;Ljava/lang/String;)V");

    jint size = _fr.size_in_bytes();
    jvmtiError  err =    env->Allocate(size,new_class_data);
    *new_class_data_len = size;
    _fr.load_to_buffer(*new_class_data);
    return;
  }

      bool loaded = false;
      if(profiler_predicate() && ok_to_track(sname)) { // for now this means when not profiling we miss classes ok  for now
      _fr.load_from_buffer(class_data);
      loaded = true;
      track_class(sname);

    }
    if (profiler_predicate() && _hooks.is_hook_class(sname) ) {
      if(!loaded) {
        _fr.load_from_buffer(class_data);
        loaded = true;
      }
      add_hooks(sname, env, new_class_data_len, new_class_data);
      _origClass[sname] = ClassPtr{class_data_len};
       env->Allocate(class_data_len, &_origClass[sname]._class_data);
       memcpy(_origClass[sname]._class_data, class_data,class_data_len);
    }

    if(!profiler_predicate() && _hooks.is_hook_class(sname) && _all_loadedClasses.count(sname) == 1) {
      _fr.load_from_buffer(class_data);
      remove_hooks(sname, env, new_class_data_len, new_class_data);
    }
    std::unique_lock<std::mutex> lck{_all_loaded_mutex};
    _all_loadedClasses.insert(sname);
}





void JNICALL MonitorContendedEnter(jvmtiEnv* env, JNIEnv* jni_env, jthread thread, jobject object)
{
  std::unique_lock<std::mutex> lck{_jammed_mutex};

  jint frame_count;
  jvmtiFrameInfo frame_info[20];
  genv->GetStackTrace(thread,0,20, frame_info, &frame_count);
  
  jvmtiMonitorUsage monitorInfo;
  genv->GetObjectMonitorUsage(object, &monitorInfo);
  jint owner_frame_count;
  jvmtiFrameInfo owner_frame_info[20];
  genv->GetStackTrace(monitorInfo.owner,0, sizeof(owner_frame_info), owner_frame_info, &owner_frame_count);

  std::string waiterStr  = formatStackTrace(genv,       frame_info,       frame_count); 
  std::string ownerStr = formatStackTrace(genv, owner_frame_info, owner_frame_count); 

  genv->Deallocate(reinterpret_cast<unsigned char*>(monitorInfo.waiters));
  genv->Deallocate(reinterpret_cast<unsigned char*>(monitorInfo.notify_waiters));
  
  _sig_mutex.lock();
  _rptr.jammed(waiterStr, ownerStr);
  _sig_mutex.unlock();
}





bool traffic_predicate() 
{
  return _commandListener._millis.load() > 0;
}

void traffic_monitor() 
{
  jvmtiEnv* xenv;
  g_java_vm->AttachCurrentThread((void**)&xenv,(void*)NULL);

  jint thread_count;
  jvmtiThreadInfo tinfo;

  char *methodName;
  char *sig;
  char *class_sig;
  char *generic;
 
  int delay = _commandListener._millis.load();
  if (!traffic_predicate()) {
    std::unique_lock<std::mutex> lck{_traffic_mutex};
    _commandListener._traffic_cond.wait(lck,&traffic_predicate);
    delay = _commandListener._millis.load();
  }
  while(1) {
    jthread* threads;
    genv->GetAllThreads(&thread_count, &threads);
    std::string rv = "";
    for(int idx = 0; idx < thread_count; idx++) {
      if((threads + idx) == NULL) continue;
      genv->GetThreadInfo(threads[idx], &tinfo);
      jint tstate;
      genv->GetThreadState(threads[idx], &tstate);

      jvmtiMonitorStackDepthInfo* monitorInfo;
      jint infoCount;
      jvmtiError ferr;

      jint frame_count;
      jvmtiFrameInfo frame_info[20];
      jint before_frame_count;
      jvmtiFrameInfo before_frame_info[20];

      memset(frame_info, 0, sizeof(frame_info));
      memset(before_frame_info, 0, sizeof(before_frame_info));

      ferr = genv->GetStackTrace(*(threads + idx) ,0,20, before_frame_info, &before_frame_count);
      if(ferr != JVMTI_ERROR_NONE)  {
        std::cout << "ERROR" << std::endl;
        continue;
      }

      infoCount = 0;
      ferr = genv->GetOwnedMonitorStackDepthInfo(*(threads + idx) , &infoCount,  &monitorInfo);  
      if(ferr != JVMTI_ERROR_NONE)  {
        std::cout << "ERROR" << std::endl;
        continue;
      }
      ferr = genv->GetStackTrace(*(threads + idx) ,0,20, frame_info, &frame_count);
      if(frame_count != before_frame_count) continue;
      
      if(ferr != JVMTI_ERROR_NONE) {
        std::cout << "ERROR" << std::endl;
        continue;
      }
      if(frame_count < 1) continue;
      std::unordered_map<int,int> monmap;
      bool invalid = false;
      for(int idx = 0; idx < infoCount; idx++, monitorInfo++) {
        monmap[(int)(monitorInfo->stack_depth)] = 1;
        if(monitorInfo->stack_depth >= frame_count) {
          invalid = true;
        }
      }
      if(invalid) continue;
      if (frame_count >= 1) {
        rv += std::string{tinfo.name} + "^";
      }
      else {
        continue;
      }
      for(int fidx = frame_count - 1; fidx >= 0; fidx--) {
        jvmtiError   err = genv->GetMethodName(frame_info[fidx].method, &methodName,&sig,&generic);
        jclass theclass;
        genv->GetMethodDeclaringClass(frame_info[fidx].method, &theclass);
        err = genv->GetClassSignature(theclass, &class_sig,&generic);
        std::string classinfo{class_sig};
        rv += (monmap.count(fidx) == 1 ? "1!" : "0!") + classinfo.substr(1) + "::";
        rv +=  std::string{methodName} + ":" + std::string{sig} + "#";
      }
      rv.erase(rv.end() -1);
      rv += "%";
    }
    if(rv.size() > 1) {
    rv.erase(rv.end() -1);
    _sig_mutex.lock();
    _rptr.traffic(rv);
    _sig_mutex.unlock();
    }
    
    if (!traffic_predicate()) {
      std::unique_lock<std::mutex> lck{_traffic_mutex};
      _commandListener._traffic_cond.wait(lck,&traffic_predicate);
      delay = _commandListener._millis.load();
    }
    else {
      std::this_thread::sleep_for(std::chrono::milliseconds(delay));
    }
    if (!traffic_predicate()) {
      std::unique_lock<std::mutex> lck{_traffic_mutex};
      _commandListener._traffic_cond.wait(lck,&traffic_predicate);
      delay = _commandListener._millis.load();
    }

        
  }
  }



bool loading_watch_predicate() 
{
  return _commandListener._load_watch_interval.load() > 0;
}

void class_loading_monitor() 
{
  jvmtiEnv* xenv;
  g_java_vm->AttachCurrentThread((void**)&xenv,(void*)NULL);

  int delay = _commandListener._load_watch_interval.load();

  if (!loading_watch_predicate()) {
    std::unique_lock<std::mutex> lck{_loading_mutex};
    _commandListener._load_watch_cond.wait(lck,&loading_watch_predicate);
    delay = _commandListener._load_watch_interval.load();
  }

  while(1) {
    _sig_mutex.lock();
    _rptr.loaded_classes(_loadedClasses);
    _loadedClasses.clear();
    _sig_mutex.unlock();
    
    if (!loading_watch_predicate()) {
      std::unique_lock<std::mutex> lck{_loading_mutex};
      _commandListener._load_watch_cond.wait(lck,&loading_watch_predicate);
      delay = _commandListener._load_watch_interval.load();
    }
    else {
      std::this_thread::sleep_for(std::chrono::milliseconds(delay));
    }
    if (!loading_watch_predicate()) {
      std::unique_lock<std::mutex> lck{_loading_mutex};
      _commandListener._load_watch_cond.wait(lck,&loading_watch_predicate);
      delay = _commandListener._load_watch_interval.load();
    }
  }
  }






void JNICALL VMInit(jvmtiEnv* env, JNIEnv* jni_env, jthread thread)
{
  std::cout << "*** INIT() " << std::endl;
  genv = env;
  gxenv = jni_env;
  jvmtiEventCallbacks* xx = new jvmtiEventCallbacks();
  xx->VMInit                = &VMInit;
  xx->ThreadStart           = &ThreadStart;
  xx->ClassFileLoadHook     = &ClassFileLoadHook;
  
  jvmtiError  err = env->SetEventNotificationMode(JVMTI_ENABLE,JVMTI_EVENT_MONITOR_WAIT,NULL);
  err = env->SetEventNotificationMode(JVMTI_ENABLE,JVMTI_EVENT_MONITOR_CONTENDED_ENTER,NULL);
  err = env->SetEventNotificationMode(JVMTI_ENABLE,JVMTI_EVENT_CLASS_FILE_LOAD_HOOK,NULL);
  std::cout << "ERR: " << err << std::endl;
  
  jint counter;
  jthread* threads;
  jvmtiThreadInfo info;
  env->GetAllThreads(&counter, &threads);
  for (int idx = 0; idx < counter; idx++ ) {
    env->GetThreadInfo(*(threads+idx), &info);
    std::cout << "thread: " << info.name << " ( " << (info.is_daemon == true) << " ) " << std::endl;
  }
  env->Deallocate((unsigned char *)threads);
  
  std::thread t1{traffic_monitor};
  t1.detach();
  std::thread t2{class_loading_monitor};
  t2.detach();
  std::thread t3{reload_monitor};
  t3.detach();
}



JNIEXPORT jint JNICALL Agent_OnLoad(JavaVM *vm, char *options, void *reserved)
{
    std::cout << "*** OnLoad() ********" << std::endl;
  g_java_vm = vm;
  std::string configDescriptor;
  if (options != NULL)
    configDescriptor = std::string{options};
  else {
    std::cout << "config file path missing from options";
    exit(0);
  }

  if(!_config.load(configDescriptor, _opcodes, _hooks)) {
    std::cout << "ERROR loading Config" << std::endl;
    exit(0);
  }

  
  if(_config.is_server_required()) {
    _conn.connect_to_server(_config.get_server_ip(), _config.get_server_port(), &_commandListener);
  }

    
  frenchroast::agent::Transport* tptr{nullptr};

  /*
   if(_config.is_cout_reporter()) {
    tptr = new frenchroast::agent::CoutTransport{};
  }

  if(_config.is_file_reporter()) {
    tptr = new frenchroast::agent::FileTransport{_config.get_report_filename()};
  }

  if(_config.is_server_reporter()) {
    tptr = new frenchroast::agent::ServerTransport{_conn};
  }
  */
  tptr = new frenchroast::agent::ServerTransport{_conn};
  _rptr.setTransport(tptr);
  jvmtiError err;
  jvmtiEnv* env;
  vm->GetEnv((void**)&env, JVMTI_VERSION);

  jvmtiEventCallbacks* xx = new jvmtiEventCallbacks();
  xx->VMInit                = &VMInit;
  xx->ThreadStart           = &ThreadStart;
  xx->MonitorContendedEnter = &MonitorContendedEnter;
  xx->ClassFileLoadHook     = &ClassFileLoadHook;
  err =   env->SetEventCallbacks(xx, sizeof(jvmtiEventCallbacks));

  jvmtiCapabilities capa;
  memset(&capa, 0, sizeof(capa));
  capa.can_generate_monitor_events = 1;
  capa.can_generate_all_class_hook_events = 1;
  capa.can_access_local_variables = 1;
  capa.can_retransform_classes = 1;
  capa.can_get_owned_monitor_stack_depth_info = 1;
  capa.can_get_monitor_info = 1;
  capa.can_suspend = 1;
  jvmtiError errcapa  = env->AddCapabilities(&capa);
  std::cout << "err capa: " << errcapa << std::endl;
  env->SetEventNotificationMode(JVMTI_ENABLE,JVMTI_EVENT_CLASS_FILE_LOAD_HOOK,NULL);
  env->SetEventNotificationMode(JVMTI_ENABLE,JVMTI_EVENT_VM_INIT,NULL);

  _rptr.ready();
  return 0;
}


JNIEXPORT void JNICALL
Agent_OnUnload(JavaVM* vm)
{

  std::cout << "unloaded" << std::endl;
  _rptr.unloaded("now");
}
  
