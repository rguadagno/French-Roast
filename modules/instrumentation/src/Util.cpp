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

#include <vector>
#include <sstream>
#include "Util.h"

namespace frenchroast {

  
  void write_bytes(BYTE* out, int value, int size)
  {
    if (size == 1) {
      *out = static_cast<BYTE>(value);
    }
    if (size == 2) {
      short vv = static_cast<short>(value);
      write_big_e_bytes(out, &vv);
    }
    
    if (size == 4) {
      write_big_e_bytes(out, &value);
    }

  }
  
  std::vector<std::string> xsplit(const std::string& str, const std::string& delim)
  {
    std::vector<std::string> rv;
    size_t pos = 0;
    size_t idx = str.length();
    size_t len = delim.length();
    while ((idx = str.find(delim,pos)) != std::string::npos) {
      rv.push_back(str.substr(pos,idx-pos));
      if (len == 1) {
        ++idx;
        pos = idx ;
      }
      else {
        idx += len-1;
        pos = idx + delim.length();
      }
       

    }
    rv.push_back(str.substr(pos-len+1,idx-(pos+len)));
    return rv;
  }


  std::vector<std::string> split(const std::string& str, const std::string& delim)
  {
    std::vector<std::string> rv;
    size_t pos = 0;
    size_t next = str.find(delim, pos);
    while ((next = str.find(delim,pos)) != std::string::npos) {
      rv.push_back(str.substr(pos,next-pos));
      pos = next + delim.length();
    }
    rv.push_back(str.substr(pos));
    return rv;
  }
  
  
  std::vector<std::string> split(const std::string& str, const char delim)
  {
    return split(str,std::string{delim});
  }

  std::vector<std::string> split(const std::string& str)
  {
    return split(str, ',');
  }

  

  void remove_blanks(std::string& line)
  {
    size_t pos;
    while ((pos=line.find(" ")) != std::string::npos) {
      line.erase(pos,1);
    }
  }

  void replace(std::string& line, const char from, const char to)
  {
    size_t pos;
    while ((pos=line.find(from)) != std::string::npos) {
      line[pos] = to;
    }
  }

  void replace(std::string& line, const char from)
  {
    size_t pos;
    while ((pos=line.find(from)) != std::string::npos) {
      line.erase(pos,1);
    }
  }

  void replace(std::string& line, const std::string& from, const std::string& to)
  {
    size_t pos;
    while ((pos=line.find(from)) != std::string::npos) {
      line.replace(pos, from.length(),to);
    }
  }

  
  int to_int(const BYTE* buf, int length)
  {
    if (length == 4) {
      int result;
      return bits_to_int<int>(result, buf);
    }
    if (length == 2) {
      short result;
      return (int)bits_to_int<short>(result, buf);
    }
    if (length == 1) {
     BYTE result;
     return (int)bits_to_int<BYTE>(result, buf);
   }
   return 0;
  }

  void incr(BYTE* buf, int length)
  {
    if (length == 4) {
      int result = to_int(buf, length) + 1;
      write_big_e_bytes(buf, &result);
      return;
    }  
    if (length == 2) {
      short result = to_int(buf, length) + 1;
      write_big_e_bytes(buf, &result);
      return;
    }  
  }

  
}
