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

#include "OpCode.h"
#include <string>

namespace frenchroast {
  

  std::unordered_map<BYTE, OpCode> OpCode::_op_codes;
  
  OpCode::OpCode(BYTE code, int size,const std::string& name) : _code(code),_size(size),_name(name),_isBranch(false), _isDynamic(false)
    {
    }
    
  OpCode::OpCode(BYTE code, int size,const std::string& name,bool isbranch,bool isdynamic) : _code(code),_size(size),_name(name),_isBranch(isbranch), _isDynamic(isdynamic)
    {
    }

    OpCode::OpCode() 
    {
    }

    bool OpCode::is_branch() const
    {
      return _isBranch;
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
  
    void OpCode::load(const std::string& fileName)
    {
      try {
	std::ifstream in;
	in.open(fileName);
	std::string line;
	while (getline(in,line)) {
	  bool isDynamic = false;
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
	  if (split(line,'<').size() > 2) 
	    _op_codes[op] = OpCode(op,osize,name,true,isDynamic);
	  else
	    _op_codes[op] = OpCode(op,osize,name,false,isDynamic);
	}
	in.close();
      }
      catch(std::ifstream::failure& e) {
	throw std::ifstream::failure("cannot open file: " + fileName);
      }
    }
  
}
  
