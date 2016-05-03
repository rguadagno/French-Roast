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
#ifndef UTIL_H
#define UTIL_H
#include <iostream>
#include <fstream>
#include <vector>
#include <memory>
#include <cstring>

namespace frenchroast {

  using BYTE =  unsigned char;
  using INDEX = BYTE[2];

  int to_int(const BYTE* buf, int length);
  void incr(BYTE* buf, int length);
  std::vector<std::string> split(const std::string& str,const char delim);
  std::vector<std::string> split(const std::string& str, const std::string&);
  std::vector<std::string> split(const std::string& str);
  void remove_blanks(std::string&);
  void replace(std::string&, char form, char to);
  void replace(std::string&, char form);
  void replace(std::string& line, const std::string& from, const std::string& to);
  std::string ntoa(int);
  std::string ntoa(double);
  std::string ntoa(long long);

  
  template
  <typename IntegerType>
  IntegerType bits_to_int(IntegerType& result, const unsigned char* bits, bool little_endian=false)
  {
    result = 0;
    if (little_endian) {
      for (int n = sizeof( result ); n >= 0;n--) {
	result = (result << 8) + bits[n];
      }
    }
    else {
      for (unsigned n = 0;n < sizeof( result ); n++) {
	result = (result << 8) + bits[n];
      }
    }
    return result;
  }

  template
  <typename IntegerType>
  void write_big_endian_bytes(std::ostream& out, IntegerType* obits, bool little_endian=false)
  {
    unsigned char* bits = (unsigned char*) obits;
    char tempout;
    for (int n = sizeof( IntegerType )-1; n >=0; n--) {
      memset(&tempout,0,sizeof(tempout));
      tempout  = (tempout << 8) +bits[n];
      out.write(&tempout,1);
    }
  }

  void write_bytes(BYTE* out, int value, int size);
  
  template
  <typename IntegerType>
  void write_big_e_bytes(BYTE* out, IntegerType* obits, bool little_endian=false)
  {
    unsigned char* bits = (unsigned char*) obits;
    char tempout;
    int idx =0;
    for (int n = sizeof( IntegerType )-1; n >=0; n--) {
      memset(&tempout,0,sizeof(tempout));
      tempout  = (tempout << 8) +bits[n]; 
      memcpy(&out[idx++], &tempout,1);
    }
  }

  std::unique_ptr<const unsigned char> get_class_buf(const std::string& fileName);
  
}

#endif
