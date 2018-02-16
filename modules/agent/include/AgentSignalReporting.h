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

#ifndef AGENTSIGREPORT_H
#define AGENTSIGREPORT_H

#include <unordered_map>
#include <vector>
#include <sstream>
#include "jni.h"
#include "jvmti.h"
#include "StackTrace.h"

namespace frenchroast { namespace agent {

    template<typename ReportType>
      std::string* to_ptr(ReportType rpt)
      {
        std::stringstream ss;
        ss << rpt;
        std::string* str = new std::string{};
        *str = ss.str();
        return str;
      }

    
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
    

std::string get_value(JNIEnv* ptr, jobject obj, FieldInfo& field);
void populate_stack( JNIEnv * jni_env, jvmtiEnv* genv, jvmtiFrameInfo* frames, int count, frenchroast::monitor::StackTrace& rv,std::unordered_map<std::string, bool>& artifacts, bool full_stack = false );

void populate_class_fields_info(jvmtiEnv* env,char* classDescriptor, jclass theclass, std::unordered_map<std::string, FieldInfo>& gfieldinfo);
void populate_artifacts(JNIEnv * ptr,  jvmtiEnv* env, jvmtiFrameInfo* frames, jobject& obj, std::vector<std::string>& params, std::vector<std::string>& fieldValues, frenchroast::monitor::StackTrace& stack, jthread& aThread);
  }
}
#endif
