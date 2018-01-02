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

#include <chrono>
#include <iostream>
#include <string>
#include <fstream>
#include <mutex>
#include <thread>
#include <atomic>
#include <condition_variable>
#include <unordered_set>
#include <unordered_map>
#include <map>
#include <boost/lockfree/queue.hpp>
#include "jvmti.h"
#include "Agent.h"
#include "AgentHooks.h"
#include "fr_signals.h"
#include "FrenchRoast.h"
#include "OpCode.h"
#include "Reporter.h"
#include "Config.h"
#include "CommandListener.h"
#include "Listener.h"
#include "AgentUtil.h"
#include "ClassDetail.h"
#include "AgentSignalReporting.h"

std::mutex _traffic_mutex;
std::mutex _loading_mutex;
std::mutex _loading_data_mutex;
std::mutex _profiler_mutex;
std::mutex _all_loaded_mutex;


boost::lockfree::queue<std::string*> _signalQueue{10000};


std::unordered_set<std::string>           _all_loadedClasses;
std::unordered_map<std::string, ClassPtr> _origClass;

frenchroast::network::Connector<>  _conn;

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
    _millis.store(interval_millis);
    _traffic_cond.notify_one();
  }

  void stop_watch_traffic()
  {
    std::cout << " ** STOP TRAFFIC **" << std::endl;
    _millis.store(-1);
    _traffic_cond.notify_one();
  }

  void stop_watch_loading()
  {
    _load_watch_interval.store(-1);
    _load_watch_cond.notify_one();

    std::cout << "** STOP WATCH LOADING **" << std::endl;
  }

  void watch_loading()
  {
    if(!profiler_predicate()) return;
    _load_watch_interval.store(5000);
    _load_watch_cond.notify_one();

    std::cout << "** START WATCH LOADING **" << std::endl;
  }

  void turn_on_profiler()
  {
    _runProfile.store(true);
    _profilerCond.notify_one();
  }

  void turn_off_profiler()
  {
    _runProfile.store(false);
    _profilerCond.notify_one();
    stop_watch_loading();
    stop_watch_traffic();
  }

  
};


struct jni_cache {
  char* meth_name;
  char* sig;
  char* classname;

};


std::vector<frenchroast::monitor::ClassDetail>                  _loadedClasses;
frenchroast::OpCode                        _opcodes;
frenchroast::FrenchRoast                   _fr{_opcodes};
frenchroast::signal::Signals               _hooks;
std::unordered_map<std::string, bool>      _artifacts;
frenchroast::agent::Config                 _config;
frenchroast::agent::Reporter               _rptr{_conn};
CommandBridge                              _commandListener;

jvmtiEnv* genv;
JavaVM*   g_java_vm;


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

      std::string* str = new std::string{};
      str->append("signaltimer~");
      str->append(std::to_string(stime));
      str->append("~");
      str->append(std::string(ptr->GetStringUTFChars(tag,0)));
      str->append("~");
      str->append(classinfo);
      str->append("::");
      str->append(methodNameStr);
      str->append(":");
      str->append(sigStr);
      str->append("~");
      str->append(std::string(ptr->GetStringUTFChars(tname,0)));
      _signalQueue.push(str);

    }
  }
}

JNIEXPORT void JNICALL Java_java_lang_Package_thook (JNIEnv * ptr, jclass klass, jobject obj)
{
  
  jvmtiFrameInfo frames[10];
  jint count;
  jthread aThread;

  memset(frames, 0, sizeof(frames));         
  if(!ErrorHandler::check_jvmti_error(genv->GetCurrentThread(&aThread),"GetCurrentThread")) return;
  std::string tname;
  if(!get_thread_name(ptr, genv, aThread, tname)) return;; 
  if(!ErrorHandler::check_jvmti_error(genv->GetStackTrace(aThread, 0, sizeof(frames), frames, &count),"GetStackTrace")) return;
  if (count >= 1) {
    std::vector<DescriptorVO>  stack;
    populate_stack(ptr, genv, frames, count, stack, _artifacts );

    std::string params = "(";
    std::string fieldValues = "";
    if(_artifacts[stack[0].descriptor()]) {
      populate_artifacts(ptr, genv, frames, obj, params, fieldValues, stack, aThread);
    }

    params.append(")");
    std::string stackstr = "";
    std::string* firstStr = new std::string{"signal~"};
    firstStr->append(stack[0].descriptor());

      
    for(auto& x : stack) {
      stackstr.append(x.descriptor());
      stackstr.append("%");
      genv->Deallocate((unsigned char*)x._methodName);
      genv->Deallocate((unsigned char*)x._methodSignature);
      genv->Deallocate((unsigned char*)x._classSignature);
    }
      
    firstStr->append("~");
    firstStr->append(tname);
    firstStr->append("~");
    firstStr->append(fieldValues);
    firstStr->append("~");
    firstStr->append(params);
    firstStr->append("~");
    firstStr->append(stackstr);
    
    _signalQueue.push(firstStr);
  }
}



void signal_sender()
{
  int buffercnt = 0;
  std::string buffer;
  std::string* signal;
  while(1) {
    while(_signalQueue.pop(signal)) {
      buffer.append(*signal);
      delete signal;
      if(++buffercnt == 9) {
        _rptr.signal(buffer);
        buffercnt = 0;
        buffer.clear();
      }
      else {
        buffer.append("<end>");
      }
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    if(buffercnt > 0) {
      _rptr.signal(buffer);
      buffercnt = 0;
      buffer.clear();
    }
  }
}

bool profiler_predicate() 
{
  return _commandListener._runProfile.load();
}


void reload_monitor() 
{
  jvmtiEnv* env;
  g_java_vm->AttachCurrentThread((void**)&env,(void*)NULL);

  JNIEnv* jni_env;
  g_java_vm->GetEnv((void**)&jni_env, JNI_VERSION_1_6);
  
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
        theclass = jni_env->FindClass(cname.c_str()); 
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

  std::vector<frenchroast::monitor::Descriptor> descriptors;
  for(auto methdesc : _fr.get_method_descriptors()) {
    if(methdesc.find("main") == std::string::npos ) {
      descriptors.emplace_back(methdesc);
    }
  }
  _loading_data_mutex.lock();
  _loadedClasses.emplace_back(name, descriptors);
  _loading_data_mutex.unlock();
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
    add_thook_to_package(_fr, class_data, env, new_class_data_len, new_class_data);
    return;
  }

  bool loaded = false;
  if(profiler_predicate() && ok_to_track(sname)) { // for now this means when not profiling we miss classes ok  for now
    _fr.load_from_buffer(class_data);
    loaded = true;
    track_class(sname);
  }
  if (profiler_predicate() && _hooks.is_signal_class(sname) ) {
    if(!loaded) {
      _fr.load_from_buffer(class_data);
      loaded = true;
    }
    add_hooks(_fr,_hooks,_artifacts,sname, env, new_class_data_len, new_class_data);
    _origClass[sname] = ClassPtr{class_data_len};
    env->Allocate(class_data_len, &_origClass[sname]._class_data);
    memcpy(_origClass[sname]._class_data, class_data,class_data_len);
  }
  
  if(!profiler_predicate() && _hooks.is_signal_class(sname) && _all_loadedClasses.count(sname) == 1) {
    _fr.load_from_buffer(class_data);
    remove_hooks(_origClass[sname]._class_data,_origClass[sname]._size, env, new_class_data_len, new_class_data);
  }
  std::unique_lock<std::mutex> lck{_all_loaded_mutex};
  _all_loadedClasses.insert(sname);
}



void JNICALL MonitorContendedEnter(jvmtiEnv* env, JNIEnv* jni_env, jthread thread, jobject object)
{
  std::string monitorStr;
  if(!get_class_name(env, jni_env->GetObjectClass(object), monitorStr)) return;
  std::string waiterStr;
  if(!format_stack_trace(env, thread, waiterStr)) return;
  jvmtiMonitorUsage monitorInfo;
  if(!ErrorHandler::check_jvmti_error(env->GetObjectMonitorUsage(object, &monitorInfo), "GetObjectMonitorUsage")) return;
  std::string ownerStr;
  if(!format_stack_trace(env, monitorInfo.owner, ownerStr)) return;
  jthread owner = monitorInfo.owner;
  jni_env->DeleteLocalRef(owner);
  delete_refs(jni_env, monitorInfo.waiters, monitorInfo.waiter_count);
  delete_refs(jni_env, monitorInfo.notify_waiters, monitorInfo.notify_waiter_count);
  env->Deallocate(reinterpret_cast<unsigned char*>(monitorInfo.waiters));
  env->Deallocate(reinterpret_cast<unsigned char*>(monitorInfo.notify_waiters));

  std::string* str = new std::string{"jammed~"};
  str->append("~");
  str->append(monitorStr);
  str->append("~");
  str->append(waiterStr);
  str->append("~");
  str->append(ownerStr);
  _signalQueue.push(str);
}


bool traffic_predicate() 
{
  return _commandListener._millis.load() > 0;
}



void traffic_monitor() 
{
  jvmtiEnv* env;
  g_java_vm->AttachCurrentThread((void**)&env,(void*)NULL);
  JNIEnv* jni_env;
  g_java_vm->GetEnv((void**)&jni_env, JNI_VERSION_1_6);

  jint thread_count;
 
  int delay = _commandListener._millis.load();
  if (!traffic_predicate()) {
    std::unique_lock<std::mutex> lck{_traffic_mutex};
    _commandListener._traffic_cond.wait(lck,&traffic_predicate);
    delay = _commandListener._millis.load();
  }

  int counter=0;
  int elapsed=0;
  std::string* rv;
  jthread thd;

  std::unordered_map<long long,struct jni_cache> cache;
  struct jni_cache lcache;
  char *methodName;
  char *sig;
  char *generic;
  char* className;
  jclass theclass;
  bool found;
  jint frame_count=0;
  jvmtiFrameInfo frame_info[20];
  jint before_frame_count;
  jvmtiFrameInfo before_frame_info[20];
  char* tname;
  long long cacheid;
  int notfoundcount = 0;
  std::unordered_map<int,int> monmap;
  while(1) {
    jthread* threads;
    thread_count=0;

    if(!ErrorHandler::check_jvmti_error(genv->GetAllThreads(&thread_count, &threads),"GetAllThreads")) continue;

    rv = new std::string{"traffic~"};
    for(int idx = 0; idx < thread_count; idx++) {
      monmap.clear();
      thd = *(threads + idx);
      if(!get_thread_name_fast(jni_env,genv, thd, &tname)) continue;
      memset(frame_info, 0, sizeof(frame_info));
      memset(before_frame_info, 0, sizeof(before_frame_info));
      jint infoCount;
      jvmtiMonitorStackDepthInfo* monitorInfo;
      if(!ErrorHandler::check_jvmti_error(genv->GetStackTrace(*(threads + idx) ,0,20, before_frame_info, &before_frame_count), "GetStackTrace")) continue;
      infoCount = 0;
      if(!ErrorHandler::check_jvmti_error(genv->GetOwnedMonitorStackDepthInfo(*(threads + idx) , &infoCount,  &monitorInfo),"GetOwnedMonitorStackDepthInfo")) continue;
      
      if(!ErrorHandler::check_jvmti_error(genv->GetStackTrace(*(threads + idx) ,0,20, frame_info, &frame_count), "GetStackTrace")) continue;

      bool invalid = false;
      for(int idx = 0; idx < infoCount; idx++) {
        monmap[((monitorInfo + idx)->stack_depth)] = 1;
        if((monitorInfo + idx)->stack_depth >= frame_count) {
          invalid = true;
        }
        jobject mon = (monitorInfo + idx)->monitor;
        jni_env->DeleteLocalRef(mon);      
        
      }

      if(frame_count != before_frame_count) {
        jni_env->DeleteLocalRef(thd);
        genv->Deallocate((unsigned char *)monitorInfo);
        continue;
      }
      if(frame_count < 1) {
        jni_env->DeleteLocalRef(thd);
        genv->Deallocate((unsigned char *)monitorInfo);
        continue;
      }
      genv->Deallocate((unsigned char *)monitorInfo);
      
      if(invalid)  {
          jni_env->DeleteLocalRef(thd);
        continue;
      }
      if (frame_count >= 1) {
        rv->append(tname);
        rv->append("^");
        genv->Deallocate((unsigned char *)tname);
      }
      else {
        genv->Deallocate((unsigned char *)tname);
        jni_env->DeleteLocalRef(thd);
        continue;
      }


      for(int fidx = frame_count - 1; fidx >= 0; fidx--) {
        generic = nullptr;
        found=false;
        cacheid = (long long)frame_info[fidx].method;
        if(cache.find(cacheid) != cache.end()) {
          found=true;
          lcache = cache[cacheid];
          methodName = lcache.meth_name;
          sig =        lcache.sig;
          className  = lcache.classname;
         }
        else {
          if(!ErrorHandler::check_jvmti_error(genv->GetMethodName(frame_info[fidx].method, &methodName,&sig,&generic), "GetMethodName")) continue;
          if(!ErrorHandler::check_jvmti_error(genv->Deallocate((unsigned char *)generic ), "Deallocate")) continue;

          if(!ErrorHandler::check_jvmti_error(genv->GetMethodDeclaringClass(frame_info[fidx].method, &theclass), "GetMethodDeclaringClass")) continue;
          if(!get_class_name_fast(genv, theclass, &className)) continue;
          jni_env->DeleteLocalRef(theclass);
        }
        rv->append((monmap.find(fidx) != monmap.end() ? "1!" : "0!"));
        rv->append(className + 1);
        rv->append( "::");

        rv->append(methodName);
        rv->append(":");
        rv->append(sig);
        rv->append("#");
     
        if(!found) {
          cache[cacheid] = {methodName, sig,className};
        }
        }
      rv->erase(rv->end() -1);
      rv->append("%");
      jni_env->DeleteLocalRef(thd);
    }
    genv->Deallocate((unsigned char *)threads);
     
    if(rv->size() > 1) {
      rv->erase(rv->end() -1);
      _signalQueue.push(rv);
    }
    else {
      delete rv;
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
    if(_loadedClasses.size() > 0 ) {
      std::string* str = new std::string{"loaded~"};
      std::stringstream ss;
      ss << _loadedClasses;
      str->append(ss.str());
      _signalQueue.push(str);
      _loadedClasses.clear();
    }
    
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
  genv = env;
  jvmtiEventCallbacks* xx = new jvmtiEventCallbacks();
  xx->VMInit                = &VMInit;
  xx->ThreadStart           = &ThreadStart;
  xx->ClassFileLoadHook     = &ClassFileLoadHook;
  
  if(!ErrorHandler::check_jvmti_error(env->SetEventNotificationMode(JVMTI_ENABLE,JVMTI_EVENT_MONITOR_WAIT,NULL), "SetEventNotificationMode")) return;
  if(!ErrorHandler::check_jvmti_error(env->SetEventNotificationMode(JVMTI_ENABLE,JVMTI_EVENT_MONITOR_CONTENDED_ENTER,NULL), "SetEventNotificationMode")) return;
  if(!ErrorHandler::check_jvmti_error(env->SetEventNotificationMode(JVMTI_ENABLE,JVMTI_EVENT_CLASS_FILE_LOAD_HOOK,NULL), "SetEventNotificationMode")) return;
  
  std::thread t1{traffic_monitor};
  t1.detach();
  std::thread t2{class_loading_monitor};
  t2.detach();
  std::thread t3{reload_monitor};
  t3.detach();
  std::thread t4{signal_sender};
  t4.detach();

  
    
  std::cout << "********************************" << std::endl;
  std::cout << "  French-Roast: Agent running" << std::endl;
  std::cout << "********************************" << std::endl;

}



JNIEXPORT jint JNICALL Agent_OnLoad(JavaVM *vm, char *options, void *reserved)
{
  std::cout << "******************************" << std::endl;
  std::cout << "  French-Roast: agent loaded" << std::endl;
  std::cout << "******************************" << std::endl;
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

  _conn.connect_to_server(_config.get_server_ip(), _config.get_server_port(), &_commandListener);
  
  jvmtiEnv* env;
  vm->GetEnv((void**)&env, JVMTI_VERSION);

  jvmtiEventCallbacks* xx = new jvmtiEventCallbacks();
  xx->VMInit                = &VMInit;
  xx->ThreadStart           = &ThreadStart;
  xx->MonitorContendedEnter = &MonitorContendedEnter;
  xx->ClassFileLoadHook     = &ClassFileLoadHook;
  if(!ErrorHandler::check_jvmti_error(env->SetEventCallbacks(xx, sizeof(jvmtiEventCallbacks)), "SetEventCallbacks")) return -1;

  jvmtiCapabilities capa;
  memset(&capa, 0, sizeof(capa));
  capa.can_generate_monitor_events = 1;
  capa.can_generate_all_class_hook_events = 1;
  capa.can_access_local_variables = 1;
  capa.can_retransform_classes = 1;
  capa.can_get_owned_monitor_stack_depth_info = 1;
  capa.can_get_monitor_info = 1;
  capa.can_suspend = 1;
  if(!ErrorHandler::check_jvmti_error(env->AddCapabilities(&capa), "AddCapabilities")) return -1;
  if(!ErrorHandler::check_jvmti_error(env->SetEventNotificationMode(JVMTI_ENABLE,JVMTI_EVENT_CLASS_FILE_LOAD_HOOK,NULL),"SetEventNotificationMode")) return -1;
  if(!ErrorHandler::check_jvmti_error(env->SetEventNotificationMode(JVMTI_ENABLE,JVMTI_EVENT_VM_INIT,NULL), "SetEventNotificationMode")) return -1;

  _rptr.ready();
  return 0;
}


JNIEXPORT void JNICALL
Agent_OnUnload(JavaVM* vm)
{

  std::cout << "unloaded" << std::endl;
  _rptr.unloaded("now");
}
  
