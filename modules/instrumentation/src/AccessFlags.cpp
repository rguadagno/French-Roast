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

#include <string>
#include <vector>
#include "AccessFlags.h"
#include "Util.h"

 
namespace  frenchroast {

  void setFlagString(std::string& str, int& flags, int flagbit, const std::string& name)
  {
    str.append(flags & flagbit ? name : "");
    flags &= ~flagbit;
  }
    
  std::string AccessFlagsComponent::getFlagString(int vflags)
  {
    std::string rv = "";
    int flags = vflags;
    setFlagString(rv, flags, ACC_PUBLIC, "ACC_PUBLIC,");
    setFlagString(rv, flags, ACC_SUPER,  "ACC_SUPER,");
    setFlagString(rv, flags, ACC_NATIVE, "ACC_NATIVE,");
    setFlagString(rv, flags, ACC_STATIC, "ACC_STATIC,");
    setFlagString(rv, flags, 0xFFFF,     "MISSING_FLAG,");
    return rv;
  }
    
  int AccessFlagsComponent::size_in_bytes() const
  {
    return sizeof(_flags);
  }

  void AccessFlagsComponent::reset()
  {
    memset(_flags, 0, sizeof(_flags));
  }

  void AccessFlagsComponent::load_from_buffer(const BYTE* buf)
  {
    memcpy(_flags, buf,sizeof(_flags));
  }
    
  void AccessFlagsComponent::load_to_buffer(BYTE* buf)
  {
    memcpy(buf, _flags, sizeof(_flags));
  }

  void AccessFlagsComponent::display(std::ostream& out)
  {
    int flgs = to_int(_flags,sizeof(_flags));
    out << getFlagString(flgs);
  }

  void AccessFlagsComponent::set(int flag)
  {
    memset(_flags,0,2);
    short local = bits_to_int<short>(local,_flags) | flag;
    write_big_e_bytes(_flags,&local);
  }

  void AccessFlagsComponent::remove(int flag)
  {
    short local = bits_to_int<short>(local,_flags);
    local &= ~flag;
    write_big_e_bytes(_flags,&local); 
  }
	    
}
