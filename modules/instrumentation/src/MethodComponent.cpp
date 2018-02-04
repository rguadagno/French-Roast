// copyright (c) 2018 Richard Guadagno
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


#include "MethodComponent.h"

namespace frenchroast {


  MethodComponent::MethodComponent(ConstantPoolComponent& pool) : _pool(pool)
  {
  }
  
  int  MethodComponent::size_in_bytes() const
  {
    int rv =2;

    for(auto& method : _methods) {
      rv += method.second.size_in_bytes();
    }
    return rv;
  }
  
  void MethodComponent::reset()
  {
    _methods.clear();
  }
     
  void MethodComponent::load_from_buffer(const BYTE* buf)
  {
    BYTE* ptr = const_cast<BYTE*>(buf);
    short total;
    to_value(ptr,total);
    auto ids = _pool.get_ids();
    for(short idx=0;idx < total;idx++) {
      MethodInfo method;
      int size = method.load_from_buffer(ptr, ids);
      ptr += size;
      _methods[_pool.get_name(method.get_name_index()) + ":" + _pool.get_name(method.get_descriptor_index())] = std::move(method);
    }
  }
     
  void MethodComponent::load_to_buffer(BYTE* buf)
  {

    short size = _methods.size();
    wbytes(buf, &size);
    auto names = _pool.get_names();
    for(auto& method : _methods) {
     buf += method.second.load_to_buffer(buf, names);
    }
  }
  
  void MethodComponent::display(std::ostream& out)
  {
  }
  
  std::vector<std::string> MethodComponent::get_method_descriptors()
  {
    std::vector<std::string> rv;
    return rv;
  }

  void MethodComponent::add_method( MethodInfo&& method)
  {
    _methods[_pool.get_name(method.get_name_index()) + ":" + _pool.get_name(method.get_descriptor_index())] = std::move(method);
  }
  
  MethodInfo& MethodComponent::get_method(const std::string& descriptor) 
  {
    return _methods[descriptor];
  }
  
  bool MethodComponent::has_method(const std::string& descriptor) const
  {
    return _methods.find(descriptor) != _methods.end();
  }
}
