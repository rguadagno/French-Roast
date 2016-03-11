#include "OpCode.h"
//#include <iostream>
#include <string>

namespace frenchroast {
  

  std::unordered_map<BYTE, OpCode> OpCode::_op_codes;
  
    OpCode::OpCode(BYTE code, int size,const std::string& name) : _code(code),_size(size),_name(name),_isBranch(false)
    {
    }
    
    OpCode::OpCode(BYTE code, int size,const std::string& name,bool isbranch) : _code(code),_size(size),_name(name),_isBranch(isbranch)
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
  
    void OpCode::load(const std::string& fileName)
    {
      try {
	std::ifstream in;
	in.open(fileName);
	std::string line;
	while (getline(in,line)) {
	  size_t pos;
	  while ((pos=line.find(" ")) != std::string::npos) {
	    line.erase(pos,1);
	  }
	  int osize = atoi(split(split(line,'<')[1],'>')[1].c_str());
	  BYTE op = static_cast<BYTE>(atoi(split(split(line,'<')[1],'>')[0].c_str()));
	  const std::string name = split(line,'<')[0];
	  if (split(line,'<').size() > 2) 
	    _op_codes[op] = OpCode(op,osize,name,true);
	  else
	    _op_codes[op] = OpCode(op,osize,name);
	}
	in.close();
      }
      catch(std::ifstream::failure& e) {
	throw std::ifstream::failure("cannot open file: " + fileName);
      }
    }
   const BYTE OpCode::aload_0       = 42;
   const BYTE OpCode::invokevirtual = 182;
   const BYTE OpCode::invokestatic  = 184;

}
  
