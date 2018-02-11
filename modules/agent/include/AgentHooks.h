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

#ifndef AGENTHOOKS_H
#define AGENTHOOKS_H

#include "jvmti.h"
#include "FrenchRoast.h"
#include "fr_signals.h"
#include "Descriptor.h"

class ClassPtr {
public:
  ClassPtr(jint size) : _size(size) {}
  ClassPtr() {}
  unsigned char* _class_data;
   jint           _size;
};

namespace frenchroast { namespace agent {
    
     const std::string HOOK_SIGNAL_DESCRIPTOR = "java/lang/Package.thook:(Ljava/lang/Object;)V";
     const std::string HOOK_TIMER_DESCRIPTOR  = "java/lang/Package.timerhook:(JLjava/lang/String;Ljava/lang/String;)V";
     const std::string HOOK_MONITOR_HEAP_DESCRIPTOR = "java/lang/Package.heaphook:(Ljava/lang/Object;)V";
  }
}

void add_thook_to_package(frenchroast::FrenchRoast& fr, const unsigned char* class_data, jvmtiEnv* env, jint* new_class_data_len, unsigned char** new_class_data);

void remove_hooks(const unsigned char* orig_class_data, jint orig_size, jvmtiEnv *env,jint* new_class_data_len, unsigned char** new_class_data);


template <typename FRType = frenchroast::FrenchRoast>
void add_hooks_to_all_methods(FRType& fr, std::bitset<4> flags)
{
  for(auto methdesc : fr.get_method_descriptors()) {
    if(methdesc.find("main") != std::string::npos) continue;
    if( methdesc.find("<init") == std::string::npos ) {
      fr.add_method_call(methdesc, frenchroast::agent::HOOK_SIGNAL_DESCRIPTOR, flags);
    }
    else {
      fr.add_method_call(methdesc, frenchroast::agent::HOOK_SIGNAL_DESCRIPTOR, frenchroast::signal::Signals::METHOD_EXIT);
    }
  }
}

template <typename FRType = frenchroast::FrenchRoast>
void add_hooks_to_all_constructors(FRType& fr, std::bitset<4> flags)
{
  for(auto methdesc : fr.get_method_descriptors()) {
 
    if(methdesc.find("main") != std::string::npos) continue;
    if( methdesc.find("<init") != std::string::npos ) {
      fr.add_method_call(methdesc, frenchroast::agent::HOOK_MONITOR_HEAP_DESCRIPTOR, flags);
    }
  }
}

  
template <typename FRType = frenchroast::FrenchRoast>
void add_hooks(FRType& fr, frenchroast::signal::Signals& hooks, std::unordered_map<std::string, bool>& artifacts, const std::string& sname, jvmtiEnv *env,jint* new_class_data_len, unsigned char** new_class_data)
{
  for (auto& x : hooks[sname]) {
    if ((x.flags() & frenchroast::signal::Signals::METHOD_TIMER) == frenchroast::signal::Signals::METHOD_TIMER) {
      fr.add_method_call(x.method_name(), frenchroast::agent::HOOK_TIMER_DESCRIPTOR, x.flags());
    }
    else {
      if(x.all()) {
        add_hooks_to_all_methods(fr,x.flags());
      }
      else if(x.monitor_heap()) {
        add_hooks_to_all_constructors(fr, x.flags());
      }
      else {
          fr.add_method_call(x.method_name(), frenchroast::agent::HOOK_SIGNAL_DESCRIPTOR, x.flags());
      }
    }
      
    jint size = fr.size_in_bytes();
    jvmtiError  err =    env->Allocate(size,new_class_data);
    *new_class_data_len = size;
  }
    
}
#endif
