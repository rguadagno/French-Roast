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

/*
struct GlobalAgentData {

  
  jrawMonitorID _lock;

};
*/

std::mutex _traffic_mutex;
frenchroast::network::Connector  _conn;

class CommandBridge : public frenchroast::agent::CommandListener, public frenchroast::network::Listener {
  
public:
  std::atomic<int> _millis{};
  std::condition_variable _traffic_cond;

    void message(const std::string& msg)
    {
      std::vector<std::string> items = frenchroast::split(msg,"~");

      if (items[0] == "stop_watch_traffic") { 
        stop_watch_traffic();
      }
      if (items.size() == 2 && items[0] == "watch_traffic") {
        watch_traffic(atoi(items[1].c_str()));
      }
    }

  
  void watch_traffic(const int interval_millis)
  {    
    std::lock_guard<std::mutex> lck{_traffic_mutex};
    _millis.store(interval_millis);
    _traffic_cond.notify_one();
  }

  void stop_watch_traffic()
  {
    std::lock_guard<std::mutex> lck{_traffic_mutex};
    _millis.store(-1);
    _traffic_cond.notify_one();
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

static int counter = 0;

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

/*
void enter_section(jvmtiEnv *env) {

  env->RawMonitorEnter(gdata->_lock);
}

void exit_section(jvmtiEnv *env) {
  env->RawMonitorExit(gdata->_lock);
}
*/

void JNICALL
ThreadStart(jvmtiEnv *env, JNIEnv* jni_env, jthread thread) {
  jvmtiThreadInfo info;
    env->GetThreadInfo(thread, &info);
    std::cout << "Thred Started: " << info.name << std::endl;
}

void JNICALL
 MonitorWait(jvmtiEnv* env, JNIEnv* jni_env, jthread thread, jobject object, jlong timeout) {
  std::cout << "*** About to get lock " << std::endl;
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
          jstring jsvalue = jstring(ovalue);
          params.append(  std::string(ptr->GetStringUTFChars(jsvalue,0)) + ",");
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
  }

  if (_hooks.is_hook_class(sname)) {
    _fr.load_from_buffer(class_data);
    for (auto& x : _hooks.get(sname)) {
      if (x.line_number() > 0) {
         _fr.add_method_call(x.method_name(), "java/lang/Package.thook:()V", x.line_number());
      }
      else {
        if ((x.flags() & frenchroast::FrenchRoast::METHOD_TIMER) == frenchroast::FrenchRoast::METHOD_TIMER) {
          _fr.add_method_call(x.method_name(), "java/lang/Package.timerhook:(JLjava/lang/String;Ljava/lang/String;)V", x.flags());
        }
        else {
          std::cout <<  x.method_name() << std::endl;
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
}

/*
void JNICALL MonitorContendedEnter(jvmtiEnv* env, JNIEnv* jni_env, jthread thread, jobject object)
{
  enter_section(env);
  jclass klass = jni_env->GetObjectClass(object);
  char* signature;
  char* generic;
  env->GetClassSignature(klass, &signature, &generic);
  // gdata->_out << "*** Blocked on : " << signature << std::endl;
  ++counter;
  exit_section(env);
}
*/


bool traffic_predicate() 
{
  return _commandListener._millis.load() > 0;
}

void traffic_monitor() 
{
  jvmtiEnv* xenv;
  g_java_vm->AttachCurrentThread((void**)&xenv,(void*)NULL);

  jvmtiStackInfo* stack_info;
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
    genv->GetAllStackTraces(10, &stack_info, &thread_count);

    std::string rv = "";
    for(int idx = 0; idx < thread_count; idx++) {
      jvmtiStackInfo* stackptr = &stack_info[idx];
      jthread thd  = stackptr->thread;
      genv->GetThreadInfo(thd, &tinfo);
      if (stackptr->frame_count >= 1) {
        rv += std::string{tinfo.name} + "^";
      }
      else {
        continue;
      }  
      for(int fidx = stackptr->frame_count - 1; fidx >= 0; fidx--) {
        jvmtiError   err = genv->GetMethodName(stackptr->frame_buffer[fidx].method, &methodName,&sig,&generic);

        jclass theclass;
        genv->GetMethodDeclaringClass(stackptr->frame_buffer[fidx].method, &theclass);
        err = genv->GetClassSignature(theclass, &class_sig,&generic);
        std::string classinfo{class_sig};
        rv += classinfo + "::";
        rv +=  std::string{methodName} + ":" + std::string{sig} + "#";
      }
      rv.erase(rv.end() -1);
      rv += "%";
    }
    
    rv.erase(rv.end() -1);
  
 
    _sig_mutex.lock();
    if(rv.find("calc") != std::string::npos)
      std::cout << rv << std::endl;
    _rptr.traffic(rv);
    _sig_mutex.unlock();
    
    
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

void JNICALL VMInit(jvmtiEnv* env, JNIEnv* jni_env, jthread thread)
{
  std::cout << "*** INIT() " << std::endl;
  genv = env;
  jvmtiEventCallbacks* xx = new jvmtiEventCallbacks();
  xx->VMInit                = &VMInit;
  xx->ThreadStart           = &ThreadStart;
  xx->MonitorWait           = &MonitorWait;
  //  xx->MonitorContendedEnter = &MonitorContendedEnter;
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
  vm->GetEnv((void**)&env, JVMTI_VERSION_1_0);

  jvmtiEventCallbacks* xx = new jvmtiEventCallbacks();
  xx->VMInit                = &VMInit;
  xx->ThreadStart           = &ThreadStart;
  xx->MonitorWait           = &MonitorWait;
  //  xx->MonitorContendedEnter = &MonitorContendedEnter;
  xx->ClassFileLoadHook     = &ClassFileLoadHook;
  err =   env->SetEventCallbacks(xx, sizeof(jvmtiEventCallbacks));

  jvmtiCapabilities capa;
  memset(&capa, 0, sizeof(capa));
  capa.can_generate_monitor_events = 1;
  capa.can_generate_all_class_hook_events = 1;
  capa.can_access_local_variables = 1;
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
  
