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

#include "AgentHooks.h"
#include "fr_signals.h"


void xput_class_buf(const std::string& fileName, unsigned char * buf, size_t size)
{
  std::ofstream klass{fileName, std::ios::binary};
  if(klass) {
    std::filebuf* rdbuf = klass.rdbuf();
    rdbuf->sputn(reinterpret_cast<char*>(buf), size);
    klass.close();
  }
  else {
    throw std::ios_base::failure("cannot open " + fileName + " for write"  );
  }
}


void add_thook_to_package(frenchroast::FrenchRoast& fr, const unsigned char* class_data, jvmtiEnv* env, jint* new_class_data_len, unsigned char** new_class_data)
{
  fr.load_from_buffer(class_data);
  
  fr.add_name_to_pool("thook");
  fr.add_name_to_pool("(Ljava/lang/Object;)V");
  fr.add_native_method("thook", "(Ljava/lang/Object;)V");
  
  fr.add_name_to_pool("timerhook");
  fr.add_name_to_pool("(JLjava/lang/String;Ljava/lang/String;)V");
  fr.add_native_method("timerhook", "(JLjava/lang/String;Ljava/lang/String;)V");

  fr.add_name_to_pool("heaphook");
  fr.add_native_method("heaphook", "(Ljava/lang/Object;)V");

  
  jint size = fr.size_in_bytes();
  jvmtiError  err =    env->Allocate(size,new_class_data);
  *new_class_data_len = size;
  fr.load_to_buffer(*new_class_data);
}


void remove_hooks(const unsigned char* orig_class_data, jint orig_size, jvmtiEnv *env,jint* new_class_data_len, unsigned char** new_class_data)
{
  jvmtiError  err =    env->Allocate(orig_size, new_class_data);
  *new_class_data_len = orig_size;
  memcpy(*new_class_data, orig_class_data, orig_size);
}


