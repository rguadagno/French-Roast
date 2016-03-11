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
//    along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
//

#include <iostream>
#include <string>
#include <fstream>
#include "jvmti.h"
#include "Monitor.h"
#include "Hooks.h"
#include "FrenchRoast.h"
#include "Util.h"

struct GlobalAgentData {

  std::ofstream _out{"c:\\temp\\out.log"};
  jrawMonitorID _lock;

};

static int counter = 0;
static GlobalAgentData g_data;
static GlobalAgentData* gdata = &g_data;
frenchroast::FrenchRoast fr;
frenchroast::monitoring::Hooks _hooks;


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

JNIEXPORT void JNICALL Java_java_lang_Package_thook (JNIEnv * ptr, jclass object)
{
  std::cout << "XXXXXXXXXX hooked XXXXXXXXXXXX" << std::endl;
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
      std::cout << "CALLED FROM: " << methodNameStr << ":" << sig << std::endl;
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
    jint size = fr.size_in_bytes();
    jvmtiError  err =    env->Allocate(size,new_class_data);
    *new_class_data_len = size;
    fr.load_to_buffer(*new_class_data); 
  }

  //  if (sname == "java/util/concurrent/ConcurrentHashMap$Segment") {
  if (_hooks.is_hook_class(sname)) {
    fr.load_from_buffer(class_data);
    fr.add_name_to_pool("thook");
    fr.add_name_to_pool("()V");
    for (auto x : _hooks.get(sname)) {
      if (x.line_number() > 0) {
	fr.add_method_call(x.method_name(), "java/lang/Package.thook:()V", x.line_number());
      }
      else {
	fr.add_method_call(x.method_name(), "java/lang/Package.thook:()V", x.flags());
      }
    }
      //    fr.add_method_call("scanAndLockForPut:(Ljava/lang/Object;ILjava/lang/Object;)Ljava/util/concurrent/ConcurrentHashMap$HashEntry;", "java/lang/Package.thook:()V", fr.METHOD_ENTER|fr.METHOD_EXIT);

    jint size = fr.size_in_bytes();
    jvmtiError  err =    env->Allocate(size,new_class_data);
    *new_class_data_len = size;
    fr.load_to_buffer(*new_class_data);
    
    //    	    std::ofstream outclass{"c:\\temp\\RG.class",std::ios_base::out | std::ios_base::binary};
    //	  outclass.write((char *)*new_class_data,size);
    //	  outclass.close();  
    
    
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
  std::string opcodeFile = "";
  std::string hooksFile  = "";
  std::string configFile;
  if (options != NULL)
    configFile = std::string{options};
  else {
    std::cout << "config file path missing from options";
    exit(0);
  }
 
  std::ifstream inconfig{configFile};
  std::string line;
  while (getline(inconfig,line)) {
    frenchroast::remove_blanks(line);
    if(frenchroast::split(line,'<')[0] == "opcodefile") {
      opcodeFile = frenchroast::split(line,'<')[1];
      opcodeFile = opcodeFile.substr(0,opcodeFile.length()-1);
    }
    if(frenchroast::split(line,'<')[0] == "hooksfile") {
      hooksFile = frenchroast::split(line,'<')[1];
      hooksFile = hooksFile.substr(0,hooksFile.length()-1);
     }
  }
  inconfig.close();
  
  if (opcodeFile == "") {
    std::cout << "opcodefile not set in config file" << std::endl;
    exit(0);
  }

  if (hooksFile == "") {
    std::cout << "hooksfile not set in config file" << std::endl;
    exit(0);
  }


  
  std::cout << "*** OnLoad() ********" << std::endl;
  try {
  fr.load_op_codes(opcodeFile);
  _hooks.load(hooksFile);
  }
  catch(std::exception& e) {
    std::cout << "ERROR: " << e.what() << std::endl;
    exit(0);
  }
  std::cout << "No Exception: "  << std::endl; 


  
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


  
  //@@ std::cout << "*** OnLoad() SetEventCallbacks " << err << std::endl;
   env->SetEventNotificationMode(JVMTI_ENABLE,JVMTI_EVENT_CLASS_FILE_LOAD_HOOK,NULL);
   env->SetEventNotificationMode(JVMTI_ENABLE,JVMTI_EVENT_VM_INIT,NULL);
   //   err = env->SetEventNotificationMode(JVMTI_ENABLE,JVMTI_EVENT_THREAD_START,NULL);
  //    err = env->SetEventNotificationMode(JVMTI_ENABLE,JVMTI_EVENT_VM_START,nullptr);
  //@@std::cout << "*** OnLoad() " << err << std::endl;

    return 0;
}



  
