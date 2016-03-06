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

#ifndef ATTRIB_H
#define ATTRIB_H

#include "ClassFileComponent.h"
#include <unordered_map>

namespace frenchroast {
    
  struct ConstantValueAttribute {
    INDEX _valueIndex; 
  };

 
  template <typename ResolverType>
  class Attribute {
    std::unordered_map<std::string, void (Attribute::*)(std::ostream&,ResolverType&) > _displayFunctions = {
      {"Code",          &Attribute<ResolverType>::display_code},
      {"ConstantValue", &Attribute<ResolverType>::display_constant_value}};
  public:
    BYTE  _length[4];
    INDEX _nameIndex; // @@ fix this, make private
    BYTE* _info;   // @@ fix this, should not really be public
    int get_length()
    {
      return to_int(_length,4);
    }

    void set_length(int length)
    {
      write_big_e_bytes(_length,&length);
    }
   
    void display_constant_value(std::ostream& out, ResolverType& resolver)
    {
      out << resolver.resolve_const(to_int(_info,2)) << std::endl;
    }

    void display_code(std::ostream& out, ResolverType& resolver)
    {
    }

    int load_from_buffer(const BYTE* buf)
    {
      int bytesRead = 0;
      memcpy(_nameIndex, buf,  sizeof(_nameIndex));
      bytesRead += sizeof(_nameIndex);
      memcpy(_length,    buf + bytesRead , sizeof(_length));
      bytesRead += sizeof(_length);
      int attrbuteLength = to_int(_length,sizeof(_length));
      _info = new BYTE[attrbuteLength];
      memcpy(_info, buf+bytesRead, attrbuteLength);
      bytesRead += attrbuteLength;
      return bytesRead;
    }

    int load_to_buffer(BYTE* buf)
    {
      int bytesWrite = 0;
      memcpy(buf,                      _nameIndex, sizeof(_nameIndex));
      bytesWrite += sizeof(_nameIndex);
      memcpy(buf + bytesWrite, _length,    sizeof(_length));
      bytesWrite += sizeof(_length);
      memcpy(buf + bytesWrite, _info,    to_int(_length,sizeof(_length)));
      bytesWrite += to_int(_length,sizeof(_length));
      return bytesWrite;
    }

    int Attribute<ResolverType>::size_in_bytes() const
    {
      return sizeof(_nameIndex) + sizeof(_length) + to_int(_length,sizeof(_length));
    }

    void display(std::ostream& out, ResolverType& resolver)
    {
      std::string name  = resolver.resolve_const(to_int(_nameIndex,sizeof(_nameIndex)));
      out << "   " <<  name << std::endl;
      if (_displayFunctions.count(name) > 0) {
	(this->*_displayFunctions[name])(out,resolver);
      }
      else {
	std::cout << "MISSING: " << name <<std::endl;
      }
    }

    Attribute(Attribute&& ref)
    {  
      memcpy(_nameIndex, ref._nameIndex, sizeof(_nameIndex));
      memcpy(_length, ref._length, sizeof(_length));
      _info = ref._info;
      ref._info=nullptr;
    }
 

   Attribute(ResolverType& resolver)
   {
   } 

   ~Attribute()
   {
     if (_info != nullptr) {
       delete[] _info;
     }
   }
   
   Attribute(const Attribute& ref) = delete;
   
   std::string get_name(ResolverType& resolver)
     {
       return resolver.resolve_const(to_int(_nameIndex,sizeof(_nameIndex)));
     }

   short get_descriptor_index()
   {
     return 0;
   }
  };

}
#endif
