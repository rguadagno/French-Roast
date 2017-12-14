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
#include "jni.h"
#include "jvmti.h"

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

class DescriptorVO {
public:
  char* _classSignature;
  char* _methodName;
  char* _methodSignature;
  DescriptorVO(char* cSig, char* mname, char* mSig) : _classSignature(cSig), _methodName(mname), _methodSignature(mSig)
  {
  }

  std::string descriptor() const
  {
    std::string rv = _classSignature;
    rv.append("::");
    rv.append(_methodName);
    rv.append(":");
    rv.append(_methodSignature);
    return rv;
  }
};


std::string get_value(JNIEnv* ptr, jobject obj, FieldInfo& field);
void populate_stack( JNIEnv * jni_env, jvmtiEnv* genv, jvmtiFrameInfo* frames, int count, std::vector<DescriptorVO>& rv,
                     std::unordered_map<std::string, bool>& artifacts );

#endif
