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
#include "jvmti.h"
#include "Agent.h"
#include "Hooks.h"
#include "FrenchRoast.h"
#include "Util.h"
#include "Reporter.h"
#include "Config.h"


struct GlobalAgentData {

  std::ofstream _out{"c:\\temp\\out.log"};
  jrawMonitorID _lock;

};

static int counter = 0;
static GlobalAgentData g_data;
static GlobalAgentData* gdata = &g_data;
frenchroast::FrenchRoast fr;
frenchroast::agent::Hooks _hooks;
frenchroast::agent::Config _config;
frenchroast::agent::Reporter _rptr;
std::mutex _sig_mutex;
std::mutex _sig_time_mutex;
jvmtiEnv* genv;

void enter_section(jvmtiEnv *env) {

  env->RawMonitorEnter(gdata->_lock);
}

void exit_section(jvmtiEnv *env) {
  env->RawMonitorExit(gdata->_lock);
}

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

JNIEXPORT void JNICALL Java_java_lang_Package_thook (JNIEnv * ptr, jclass object)
{
  jvmtiFrameInfo frames[5];
  jint count;
  jvmtiError err;
  jthread aThread;

  genv->GetCurrentThread(&aThread);
  jvmtiThreadInfo info;
  genv->GetThreadInfo(aThread, &info);
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
      _sig_mutex.lock();
      _rptr.signal(classinfo + "::" + methodNameStr + ":" + sigStr + "~" + std::string{info.name});
      _sig_mutex.unlock();
    }
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
    fr.load_from_buffer(class_data);
    
    fr.add_name_to_pool("thook");
    fr.add_name_to_pool("()V");
    fr.add_native_method("thook", "()V");

    fr.add_name_to_pool("timerhook");
    fr.add_name_to_pool("(JLjava/lang/String;Ljava/lang/String;)V");
    fr.add_native_method("timerhook", "(JLjava/lang/String;Ljava/lang/String;)V");

    jint size = fr.size_in_bytes();
    jvmtiError  err =    env->Allocate(size,new_class_data);
    *new_class_data_len = size;
    fr.load_to_buffer(*new_class_data); 
  }

  if (_hooks.is_hook_class(sname)) {
    fr.load_from_buffer(class_data);
    for (auto& x : _hooks.get(sname)) {
      if (x.line_number() > 0) {
	 fr.add_method_call(x.method_name(), "java/lang/Package.thook:()V", x.line_number());
      }
      else {
	if ((x.flags() & frenchroast::FrenchRoast::METHOD_TIMER) == frenchroast::FrenchRoast::METHOD_TIMER) {
	  fr.add_method_call(x.method_name(), "java/lang/Package.timerhook:(JLjava/lang/String;Ljava/lang/String;)V", x.flags());
	}
	else {
	  fr.add_method_call(x.method_name(), "java/lang/Package.thook:()V", x.flags());
	}
      }
    jint size = fr.size_in_bytes();
    jvmtiError  err =    env->Allocate(size,new_class_data);
    *new_class_data_len = size;
    fr.load_to_buffer(*new_class_data);
    }
  }
}


void JNICALL MonitorContendedEnter(jvmtiEnv* env, JNIEnv* jni_env, jthread thread, jobject object)
{
  enter_section(env);
  jclass klass = jni_env->GetObjectClass(object);
  char* signature;
  char* generic;
  env->GetClassSignature(klass, &signature, &generic);
  gdata->_out << "*** Blocked on : " << signature << std::endl;
  ++counter;
  exit_section(env);
}

void JNICALL VMInit(jvmtiEnv* env, JNIEnv* jni_env, jthread thread)
{
  std::cout << "*** INIT() " << std::endl;
  genv = env;
  jvmtiEventCallbacks* xx = new jvmtiEventCallbacks();
  xx->VMInit                = &VMInit;
  xx->ThreadStart           = &ThreadStart;
  xx->MonitorWait           = &MonitorWait;
  xx->MonitorContendedEnter = &MonitorContendedEnter;
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
}




JNIEXPORT jint JNICALL Agent_OnLoad(JavaVM *vm, char *options, void *reserved)
{
  std::string configFile;
  if (options != NULL)
    configFile = std::string{options};
  else {
    std::cout << "config file path missing from options";
    exit(0);
  }



  try {
    if (!_config.load(configFile)) {
      exit(0);
    }
  }
  catch(std::ifstream::failure& e) {
    std::cout << e.what() << std::endl;
    exit(0);
  }

  
  std::cout << "*** OnLoad() ********" << std::endl;
  try {
    fr.load_op_codes(_config.get_opcode_file());
    _hooks.load(_config.get_hooks_file());
  }
  catch(std::exception& e) {
    std::cout << "ERROR: " << e.what() << std::endl;
    exit(0);
  }

  std::cout << "===========> " << _config.get_reporter_descriptor() << std::endl;
  
    _rptr.init(_config.get_reporter_descriptor());
  jvmtiError err;
  jvmtiEnv* env;
  vm->GetEnv((void**)&env, JVMTI_VERSION_1_0);

  jvmtiEventCallbacks* xx = new jvmtiEventCallbacks();
  xx->VMInit                = &VMInit;
  xx->ThreadStart           = &ThreadStart;
  xx->MonitorWait           = &MonitorWait;
  xx->MonitorContendedEnter = &MonitorContendedEnter;
  xx->ClassFileLoadHook     = &ClassFileLoadHook;
  err =   env->SetEventCallbacks(xx, sizeof(jvmtiEventCallbacks));

  jvmtiCapabilities capa;
  capa.can_generate_monitor_events = 1;
  capa.can_generate_all_class_hook_events = 1;
  jvmtiError errcapa  = env->AddCapabilities(&capa);
  std::cout << "err capa: " << errcapa << std::endl;
  env->SetEventNotificationMode(JVMTI_ENABLE,JVMTI_EVENT_CLASS_FILE_LOAD_HOOK,NULL);
  env->SetEventNotificationMode(JVMTI_ENABLE,JVMTI_EVENT_VM_INIT,NULL);
  return 0;
}



  
