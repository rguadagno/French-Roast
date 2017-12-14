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

class ClassPtr {
public:
  ClassPtr(jint size) : _size(size) {}
  ClassPtr() {}
  unsigned char* _class_data;
   jint           _size;
};

void add_thook_to_package(frenchroast::FrenchRoast& fr, const unsigned char* class_data, jvmtiEnv* env, jint* new_class_data_len, unsigned char** new_class_data);

void remove_hooks(const unsigned char* orig_class_data, jint orig_size, jvmtiEnv *env,jint* new_class_data_len, unsigned char** new_class_data);

template <typename FRType = frenchroast::FrenchRoast>
void add_hooks(FRType& fr, frenchroast::signal::Signals& hooks, std::unordered_map<std::string, bool>& artifacts, const std::string& sname, jvmtiEnv *env,jint* new_class_data_len, unsigned char** new_class_data)
{
    for (auto& x : hooks[sname]) {
    artifacts["L" + sname + ";::" + x.method_name()] = x.artifacts();
    if (x.line_number() > 0) {
      fr.add_method_call(x.method_name(), "java/lang/Package.thook:()V", x.line_number());
    }
    else {
      if ((x.flags() & frenchroast::FrenchRoast::METHOD_TIMER) == frenchroast::FrenchRoast::METHOD_TIMER) {
        fr.add_method_call(x.method_name(), "java/lang/Package.timerhook:(JLjava/lang/String;Ljava/lang/String;)V", x.flags());
      }
      else {
        if(x.all()) {
          for(auto methdesc : fr.get_method_descriptors()) {
            if(methdesc.find("main") == std::string::npos         ) {
              if( methdesc.find("<init") == std::string::npos ) {
                fr.add_method_call(methdesc, "java/lang/Package.thook:(Ljava/lang/Object;)V", x.flags());
              }
              else {
                  fr.add_method_call(methdesc, "java/lang/Package.thook:(Ljava/lang/Object;)V", frenchroast::FrenchRoast::METHOD_EXIT);
              }
            }
          }
        }
        else {
          fr.add_method_call(x.method_name(), "java/lang/Package.thook:(Ljava/lang/Object;)V", x.flags());
        }
      }
    }
    jint size = fr.size_in_bytes();
    jvmtiError  err =    env->Allocate(size,new_class_data);
    *new_class_data_len = size;
    fr.load_to_buffer(*new_class_data);
  }

}
#endif
