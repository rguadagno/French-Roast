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

#ifndef INFO_H
#define INFO_H
#include "AccessFlags.h"
#include "Attribute.h"
#include <string>

namespace frenchroast {
    
  template <typename ResolverType>
  class Info {
    AccessFlagsComponent   _accessFlags;
    INDEX  _nameIndex; 
    INDEX  _descriptorIndex;
    BYTE   _attributesCount[2];
    std::vector<Attribute<ResolverType>>  _attributes; 
    std::unordered_map<std::string,int> _attribute_index_map;
    ResolverType& _resolver; 
  public:
    Info(ResolverType& resolver)  : _resolver(resolver)
    {
      memset(_attributesCount,0,sizeof(_attributesCount));
    }

    void set_flags(short flags)
    {
      _accessFlags.set(flags);
    }

    void set_name_index(short idx)
    {
      write_big_e_bytes(_nameIndex, &idx);
    }

    void set_descriptor_index(short idx)
    {
      write_big_e_bytes(_descriptorIndex, &idx);
    }

    Attribute<ResolverType>& get_attribute(const std::string name)
    {
      return _attributes[_attribute_index_map[name]];
    }

    int get_attribute_count()
    {
      return _attributes.size();
    }

    int load_from_buffer(const BYTE* buf)
    {
      int bytesRead = 0;
      _accessFlags.load_from_buffer(buf);
      bytesRead += _accessFlags.size_in_bytes();
      memcpy(_nameIndex,    buf + bytesRead , sizeof(_nameIndex));
      bytesRead += sizeof(_nameIndex);
      memcpy(_descriptorIndex,    buf + bytesRead , sizeof(_descriptorIndex));
      bytesRead += sizeof(_descriptorIndex);
      memcpy(_attributesCount,    buf + bytesRead , sizeof(_attributesCount));
      bytesRead += sizeof(_attributesCount);
      _attributes.reserve(to_int(_attributesCount,sizeof(_attributesCount)));
      for (int idx=0;idx < to_int(_attributesCount,sizeof(_attributesCount));idx++) {
        Attribute<ResolverType> attrib{_resolver};
        bytesRead +=  attrib.load_from_buffer(buf + bytesRead);
        _attributes.push_back(std::move(attrib));
        _attribute_index_map[ attrib.get_name(_resolver)  ] = idx;
      }
      return bytesRead;
    }

    int load_to_buffer(BYTE* buf)
    {
      int bytesWrite =0;
      _accessFlags.load_to_buffer(buf);
      bytesWrite += _accessFlags.size_in_bytes();
      memcpy(buf + bytesWrite,          _nameIndex, sizeof(_nameIndex));
      bytesWrite += sizeof(_nameIndex);
      memcpy(buf + bytesWrite,          _descriptorIndex, sizeof(_descriptorIndex));
      bytesWrite += sizeof(_descriptorIndex);
      memcpy(buf + bytesWrite,          _attributesCount, sizeof(_attributesCount));
      bytesWrite += sizeof(_attributesCount);
      for (auto& x : _attributes) {
      	bytesWrite += x.load_to_buffer(buf + bytesWrite);
      }
      return bytesWrite;
    }

    int size_in_bytes() const
    {
      int rv=0;
      for (auto& x : _attributes) {
	rv+=x.size_in_bytes();
      }
      rv += _accessFlags.size_in_bytes() + sizeof(_nameIndex) + sizeof(_attributesCount) + sizeof(_descriptorIndex);
      return rv;
    }

    void display(std::ostream& out, ResolverType& resolver)
    {
      out << resolver.resolve_const(to_int(_nameIndex,sizeof(_nameIndex)))  << ":";
      out << resolver.resolve_const(to_int(_descriptorIndex,sizeof(_descriptorIndex)))  << "    ";
      _accessFlags.display(out);
      out << std::endl;
      for(auto& x : _attributes) {
	x.display(out,resolver);
      }
      out << std::endl;
    }

    std::string get_name(ResolverType& resolver) const
      {
	return resolver.resolve_const(to_int(_nameIndex,sizeof(_nameIndex))) + ":" + resolver.resolve_const(to_int(_descriptorIndex,sizeof(_descriptorIndex)));
      }

    void set_attribute_value(const std::string& name, BYTE* value)
    {
      <Attribute<ResolverType> attrib = _attributes[_attribute_index_map[name]];  
      attrib.setValueBytes(value);
    }
   
    ConstantValueAttribute* get_constant_value_attribute()
    {
      return reinterpret_cast<ConstantValueAttribute*>(_attributes[_attribute_index_map["ConstantValue"]]._info);
    }
  };
}
#endif
