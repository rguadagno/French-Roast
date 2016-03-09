#ifndef OP_H
#define OP_H
#include <unordered_map>
#include "ClassFileComponent.h"

namespace frenchroast {
  class OpCode {
    BYTE         _code;
    int          _size;
    std::string _name;
    bool        _isBranch;
    static std::unordered_map<BYTE, OpCode> _op_codes;    
  public:
    OpCode(BYTE code, int size,const std::string& name);
    OpCode(BYTE code, int size,const std::string& name,bool isbranch);
    OpCode();
    OpCode& operator[](BYTE op);
    bool is_branch() const;
    int get_size() const;
    std::string get_name() const;
    BYTE get_code() const;
    void load(const std::string& fileName);
    static  const BYTE aload_0;
    static const BYTE invokevirtual;
    static const BYTE invokestatic;
  };
}


#endif
