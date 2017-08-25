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
#include "OpCode.h"

namespace frenchroast {
  

  std::unordered_map<BYTE, OpCode> OpCode::_op_codes;
  
  OpCode::OpCode(BYTE code, int size,const std::string& name, const std::bitset<4> attributes) : _code(code),_size(size),_name(name),_attributes(attributes), _isDynamic(false)
    {
    }
    
  OpCode::OpCode(BYTE code, int size,const std::string& name, const std::bitset<4> attributes,bool isdynamic) : _code(code),_size(size),_name(name),_attributes(attributes), _isDynamic(isdynamic)
    {
    }

    OpCode::OpCode() 
    {
    }

    bool OpCode::is_branch() const
    {
      return (_attributes & Branch) == Branch;
    }

  bool OpCode::is_raw() const
    {
      return (_attributes & Raw) == Raw;
    }

    int OpCode::get_size() const
    {
      return _size;
    }

    std::string OpCode::get_name() const
    {
      return _name;
    }
    
    BYTE OpCode::get_code() const
    {
      return _code;
    }

    OpCode::operator BYTE() const
    {
      return _code;
    }
  
    OpCode& OpCode::operator[](BYTE op)
    {
      if(_op_codes.count(op) == 0) {
        std::cout << "MISSING OPCODE: " << (int)op << std::endl;op = 0;
        exit(0);
      }
      return _op_codes[op];
    }

    bool OpCode::is_dynamic() const
    {
      return _isDynamic;
    }


   void OpCode::load_from_file(const std::string& fileName)
   {
    try {
        std::ifstream in;
        in.open(fileName);
        std::string line;
        while (getline(in,line)) {
          load(line);
        }
        in.close();
      }
    catch(std::ifstream::failure& ) {
        throw std::ifstream::failure("cannot open file: " + fileName);
      }
   }
  
    void OpCode::load(std::string line)
    {
      bool isDynamic = false;
      std::bitset<4> attributes{None};
      size_t pos;
      while ((pos=line.find(" ")) != std::string::npos) {
        line.erase(pos,1);
      }
      std::string sz{split(split(line,'<')[1],'>')[1]};
      
      
      int osize = 0;
      if (sz  == "*") {
        isDynamic = true;
      }
      else {
        osize = atoi(sz.c_str());  
      }
      
      BYTE op = static_cast<BYTE>(atoi(split(split(line,'<')[1],'>')[0].c_str()));
      const std::string name = split(line,'<')[0];
      if(line.find("<branch>") != std::string::npos)
        attributes = Branch;
      if(line.find("<raw>") != std::string::npos)
        attributes = Raw;
      _op_codes[op] = OpCode(op,osize,name,attributes,isDynamic);
    }
  const std::bitset<4> OpCode::None{"0000"};
  const std::bitset<4> OpCode::Branch{"0001"};
  const std::bitset<4> OpCode::Raw{"0010"};
}
  
