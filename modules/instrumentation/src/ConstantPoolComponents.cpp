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
//    along with French-Roast.  If not, see <http://www.gnu.org/licenses/>.
//

#include <sstream>
#include <exception>
#include <cmath>
#include "FrenchRoast.h"
#include "ConstantPool.h"
#include "Util.h"


namespace frenchroast {
    
  class MethodRefHolder : public InfoHolder {
    BYTE              _classIndex[2];
    BYTE              _nameTypeIndex[2];
  public:
    MethodRefHolder(const BYTE* buf)
    {
      _name = "MethodRef";
      _tag = TAG_MethodRef;
      memcpy(_classIndex,    buf,                       sizeof(_classIndex));
      memcpy(_nameTypeIndex, buf + sizeof(_classIndex), sizeof(_nameTypeIndex));
    }

    MethodRefHolder(short classIndex, short nametypeIndex)
    {
      _name = "MethodRef";
      _tag = TAG_MethodRef;
      write_big_e_bytes(_classIndex, &classIndex);
      write_big_e_bytes(_nameTypeIndex, &nametypeIndex);
    }

    int size_in_bytes() const
    {
      return sizeof(_classIndex) + sizeof(_nameTypeIndex);
    }

    std::string get_params() const
    {
      std::ostringstream ss;
      short result;
      ss << bits_to_int<short>(result,_classIndex) << "." << bits_to_int<short>(result,_nameTypeIndex); 
      return ss.str();
    }
   
    void load_to_buffer(BYTE* buf) {
      memcpy(buf,                       _classIndex,    sizeof(_classIndex));
      memcpy(buf + sizeof(_classIndex), _nameTypeIndex, sizeof(_nameTypeIndex));
    }
  };

  class InterfaceMethodRefHolder : public MethodRefHolder {
  public:
    InterfaceMethodRefHolder(const BYTE* buf) : MethodRefHolder(buf)
    {
      _name = "InterfaceMethodRef";
      _tag = TAG_MethodRef;
    }
  };

  class FieldRefHolder : public InfoHolder {
    BYTE              _classIndex[2];
    BYTE              _nameTypeIndex[2];
  public:
    FieldRefHolder(const BYTE* buf)
    {
      _name = "FieldRef";
      memcpy(_classIndex,    buf,                       sizeof(_classIndex));
      memcpy(_nameTypeIndex, buf + sizeof(_classIndex), sizeof(_nameTypeIndex));
    }

    FieldRefHolder(short classIndex, short nametypeIndex)
    {
      _name = "FieldRef";
      _tag = TAG_FieldRef;
      write_big_e_bytes(_classIndex, &classIndex);
      write_big_e_bytes(_nameTypeIndex, &nametypeIndex);
    }

    
    int size_in_bytes() const
    {
      return sizeof(_classIndex) + sizeof(_nameTypeIndex);
    }

    std::string get_params() const
    {
      std::ostringstream ss;
      short result;
      ss << bits_to_int<short>(result,_classIndex) << "." << bits_to_int<short>(result,_nameTypeIndex); 
      return ss.str();
    }
       
    void load_to_buffer(BYTE* buf)
    {
      memcpy(buf,                       _classIndex,    sizeof(_classIndex));
      memcpy(buf + sizeof(_classIndex), _nameTypeIndex, sizeof(_nameTypeIndex));
    }
  };

  IntegerHolder::IntegerHolder(int value)
  {
    _name = "Integer";
    _tag  = TAG_Integer;
    write_big_e_bytes(_bytes,&value);
  }

  IntegerHolder::IntegerHolder(const BYTE* buf)
  {
    _tag  = TAG_Integer;
    _name = "Integer";
    memcpy(_bytes,buf, sizeof(_bytes));
  }
      
  int IntegerHolder::size_in_bytes() const
  {
    return sizeof(_bytes);
  }

  std::string IntegerHolder::get_params() const
  {
    std::ostringstream ss;
    ss << to_int(_bytes, sizeof(_bytes));
    return ss.str();
  }
       
  void IntegerHolder::load_to_buffer(BYTE* buf)
  {
    memcpy(buf, _bytes,    sizeof(_bytes));
  }

  class ClassHolder : public InfoHolder {
   BYTE _classIndex[2];
  public:
    ClassHolder(const BYTE* buf)
    {
      memcpy(_classIndex, buf,   sizeof(_classIndex));
      _tag = TAG_Class;
      _name = "Class";
    }

    ClassHolder(short idx)
    {
      write_big_e_bytes(_classIndex,&idx); 
      _tag = TAG_Class;
      _name = "Class";
    }

    std::string get_params() const
    {
      std::ostringstream ss;
      short result;
      ss << bits_to_int<short>(result,_classIndex) ; 
      return ss.str();
    }

    int size_in_bytes() const
    {
      return sizeof(_classIndex);
    }

    void load_to_buffer(BYTE* buf)
    {
      memcpy(buf, _classIndex,   sizeof(_classIndex)); }
    
  };

  class StringHolder : public InfoHolder {
      BYTE _index[2];
  public:
    StringHolder(const BYTE* buf)
    {
      memcpy(_index, buf,   sizeof(_index));
      _tag = TAG_String;
      _name = "String";
    }

    StringHolder(short idx)
    {
      write_big_e_bytes(_index,&idx); 
      _tag = TAG_String;
      _name = "String";
    }
    
    std::string get_params() const
    {
      std::ostringstream ss;
      short result;
      ss << bits_to_int<short>(result,_index) ; 
      return ss.str();
    }

    int size_in_bytes() const
    {
      return sizeof(_index);
    }

    void load_to_buffer(BYTE* buf)
    {
      memcpy(buf, _index,   sizeof(_index));
    }
  };

  class Utf8Holder : public InfoHolder {

    std::vector<BYTE>          _data;      
  public:
    BYTE                       _length[2];
    Utf8Holder(const BYTE* bufstart)
    {
      _name = "Utf8";
      memcpy(_length,bufstart,sizeof(_length));
      for (short idx = sizeof(_length); idx <= to_int(_length,sizeof(_length))+1; idx++) {
        _data.push_back(*(bufstart + idx));
      }
    }
      
    std::string get_params() const
    {
      std::ostringstream ss;
      for (const auto& x : _data) {
        ss << x ;
      }
      return ss.str();
    }
  
    void load_to_buffer(BYTE* buf) {
      memcpy(buf, _length, sizeof(_length));
      int idx = sizeof(_length);
      for (auto& x : _data) {
        memcpy(buf + (idx++), &x, sizeof(x));
      }
    }
  
    int size_in_bytes() const
    {
      return static_cast<int>(sizeof(_length) + _data.size());
    }
  };
    
  class NameAndTypeHolder : public InfoHolder {
    BYTE _nameIndex[2];
    BYTE _descriptorIndex[2];
  public:
    NameAndTypeHolder(const BYTE* buf)
    {
      _name = "NameAndType";
      _tag  = TAG_NameAndType;
      memcpy(_nameIndex,      buf,                      sizeof(_nameIndex));
      memcpy(_descriptorIndex,buf + sizeof(_nameIndex), sizeof(_descriptorIndex));
    }

    NameAndTypeHolder(short nameidx, short typeidx)
    {
      _name = "NameAndType";
      _tag  = TAG_NameAndType;
      write_big_e_bytes(_nameIndex,&nameidx);
      write_big_e_bytes(_descriptorIndex,&typeidx);
    }
          
    std::string get_params() const
    {
      std::ostringstream ss;
      ss << to_int(_nameIndex,sizeof(_nameIndex)) << "." << to_int(_descriptorIndex,sizeof(_descriptorIndex)); 
      return ss.str();
    }

    int size_in_bytes() const
    {
      return sizeof(_nameIndex) + sizeof(_descriptorIndex);
    }

    void load_to_buffer(BYTE* buf)
    {
      memcpy(buf,                      _nameIndex,       sizeof(_nameIndex));
      memcpy(buf + sizeof(_nameIndex), _descriptorIndex, sizeof(_descriptorIndex));
    }
  };

  class LongHolder : public InfoHolder {
    BYTE _high[4];
    BYTE _low[4];
  public:
    LongHolder(const BYTE* buf)
    {
      _name = "Long";
      memcpy(_high,    buf,                 sizeof(_high));
      memcpy(_low,     buf + sizeof(_high), sizeof(_low));
    }
    
    int size_in_bytes() const
    {
      return sizeof(_high) + sizeof(_low);
    }

    std::string get_params() const
    {
      std::ostringstream ss;
      unsigned long long bits = to_int(_high,sizeof(_high)); 
      bits = (bits << 32) + to_int(_low,sizeof(_low));
      int s = (bits >> 63) == 0 ? 1 : -1;
      int e = (int)((bits >> 52) & 0x7ffL);
      auto m = (e == 0) ? (bits & 0xfffffffffffffL) << 1 : ((bits & 0xfffffffffffffL) | 0x10000000000000L);
      double dd = (double)(s * m * pow(2,e-1075));
      ss << dd; 
      return ss.str();
    }

    void load_to_buffer(BYTE* buf) {
       memcpy(buf,                 _high, sizeof(_high));
       memcpy(buf + sizeof(_high), _low,  sizeof(_low));
     }

  };
    
  class DoubleHolder : public InfoHolder {
    BYTE _high[4];
    BYTE _low[4];
  public:
    DoubleHolder(const BYTE* buf)
    {
      _name = "Double";
      memcpy(_high,    buf,                 sizeof(_high));
      memcpy(_low,     buf + sizeof(_high), sizeof(_low));
    }

    int size_in_bytes() const
    {
      return sizeof(_high) + sizeof(_low);
    }

    std::string get_params() const
    {
      std::ostringstream ss;
      unsigned long long bits = to_int(_high,sizeof(_high)); 
      bits = (bits << 32) + to_int(_low,sizeof(_low));
      int s = (bits >> 63) == 0 ? 1 : -1;
      int e = (int)((bits >> 52) & 0x7ffL);
      auto m = (e == 0) ? (bits & 0xfffffffffffffL) << 1 : ((bits & 0xfffffffffffffL) | 0x10000000000000L);
      double dd = (double)(s * m * pow(2,e-1075));
      ss << dd; 
      return ss.str();
    }

    void load_to_buffer(BYTE* buf)
    {
      memcpy(buf,                 _high, sizeof(_high));
      memcpy(buf + sizeof(_high), _low,  sizeof(_low));
    }
  };

  class EmptyHolder : public InfoHolder {
  public:
    EmptyHolder() {}
    std::string get_params() const { return "EMPTY"; }
    int size_in_bytes() const { return 0;   }
    void load_to_buffer(BYTE* buf) { }
  };

    

  PoolInfo::PoolInfo(const PoolInfo& ref)
  {
  }

  PoolInfo::PoolInfo(unsigned char tag, int idx, InfoHolder* holder) : _tag(tag), _index(idx), _holder(holder)
  {
  }

  PoolInfo::PoolInfo(PoolInfo&& ref)
  {
    _index = ref._index;
    _tag = ref._tag;
    _holder = ref._holder;
    ref._holder = nullptr;
  }

  int PoolInfo::get_tag()
  {
    return static_cast<int>(_tag);
  }

  std::string PoolInfo::get_params() const
  {
    return _holder->get_params();
  }

  PoolInfo::~PoolInfo()
  {
    if (_holder != nullptr) {
      delete _holder;
    }
  }

  int PoolInfo::size_in_bytes() const
  {
    return sizeof(_tag) + _holder->size_in_bytes();
  }
    
  void PoolInfo::load_to_buffer(BYTE* buf)
  {
    memcpy(buf, &_tag, sizeof(_tag));
    _holder->load_to_buffer(buf + sizeof(_tag));
  }
      
  int PoolInfo::index() const
  {
    return _index;
  }
   
  int ConstantPoolComponent::size_in_bytes() const
  {
    int rv = sizeof(_bcount);
    for (auto& x : _info) {
      rv += x.size_in_bytes();
    }
    return rv;
  }
  
  class MYERROR : public std::exception {
    std::string _desc;
  public:
    MYERROR(const std::string& desc) : _desc(desc)
    {
    }
  };
    
  InfoHolder* ConstantPoolComponent::build_holder(unsigned char tag, const unsigned char* buf)
  {
    switch(tag) {
    case TAG_MethodRef:            return new MethodRefHolder(buf);
    case TAG_InterfaceMethodRef:   return new InterfaceMethodRefHolder(buf);
    case TAG_FieldRef:             return new FieldRefHolder(buf);
    case TAG_Integer:              return new IntegerHolder(buf);
    case TAG_Class:                return new ClassHolder(buf);
    case TAG_Utf8:                 return new Utf8Holder(buf);
    case TAG_NameAndType:          return new NameAndTypeHolder(buf);
    case TAG_Double:               return new DoubleHolder(buf);
    case TAG_Long:                 return new LongHolder(buf);
    case TAG_String:               return new StringHolder(buf);
      
    default:
      std::cout << "                        BAD CONST POOL TAG " << (int)tag << std::endl;
      return new EmptyHolder();
    };
  }


  std::unordered_map<int,std::string> ConstantPoolComponent::get_ids() const
  {
    return _ids;
  }

  std::unordered_map<std::string,int> ConstantPoolComponent::get_names() const
  {
    return _names;
  }

  void ConstantPoolComponent::reset()
  {
    memset(_bcount, 0, sizeof(_bcount));
    _info.clear();
    _nextIndex=0;
    _names.clear();
  }

    
  int ConstantPoolComponent::operator+(InfoHolder* ptr)
  {
    _count++;
    short localbcount = to_int(_bcount,sizeof(_bcount))+1;
    write_big_e_bytes<short>(_bcount, &localbcount);
    bool skip;
    add_info(ptr->get_tag(),ptr,skip);
    return next_index();
  }

  int ConstantPoolComponent::add_info_from_raw( BYTE* buf,bool& skip)
  {
    return add_info_from_raw(0,buf,skip);
  }

  int ConstantPoolComponent::add_info_from_raw(int offset, const BYTE* buf,bool& skip)
  {
    unsigned char tag = buf[offset];
    InfoHolder* item = build_holder(tag, &buf[offset+1]);
    int rv = offset + add_info(tag,item,skip);
    return rv;
  }
    
  int ConstantPoolComponent::add_info(BYTE tag, InfoHolder* item, bool& skip)
  {
    if (tag == TAG_Double || tag == TAG_Long) {
      --_count;
      skip = true;
    }
    if(_info.size() == _count) {
        return 0;
    }  
    _names[item->get_params()] = ++_nextIndex;
    _ids[_nextIndex] = item->get_params();
    _info.push_back(PoolInfo{tag, _nextIndex, item});
    if (tag == TAG_Double || tag == TAG_Long) {
      ++_nextIndex;
    }
    return item->size_in_bytes();
  }

  int ConstantPoolComponent::next_index() const {
    return _nextIndex;
  }
    
  std::string ConstantPoolComponent::resolve_name(int id)
  {
    if (_ids.count(id) == 0) {
      throw std::range_error("resolveName, index id not found: " + id);
    }
    return _ids[id]; 
  }

  void ConstantPoolComponent::validate_method_name(const std::string& name)
  {
    if (name.find_first_of('.') == std::string::npos) {
      throw std::invalid_argument{"bad method name format"};
    }

    if (name.find_first_of(':') == std::string::npos) {
      throw std::invalid_argument{"bad method name format"};
    }
  }
    
  int ConstantPoolComponent::get_method_ref_index(const std::string& name)
  {
    validate_method_name(name);
    std::string classstr = split(name, '.')[0];
    std::string namestr = split(split(name, '.')[1],':')[0];
    std::string typestr = split(split(name, '.')[1],':')[1];

    int classIndex = _names[std::to_string(_names[classstr])];
    if(classIndex == 0) {
      return 0;
    }
    int nameAndTypeIndex = _names[std::to_string(_names[namestr]) + "." + std::to_string(_names[ typestr ])];
    if(nameAndTypeIndex == 0) {
      return 0;
    }
    return _names[std::to_string(classIndex) + "." + std::to_string(nameAndTypeIndex)];;
  }

  int ConstantPoolComponent::add_class(const std::string& tname)
  {
    return add_class(add_name(tname));
  }

  int ConstantPoolComponent::add_class(int nameid)
  {
    *this + new ClassHolder(nameid);
    return next_index();
  }

  int ConstantPoolComponent::add_string(const std::string& sname)
  {
    *this + new StringHolder(add_name(sname));
    return next_index();    
  }

  int ConstantPoolComponent::add_field_ref_index(int class_index, const std::string& name)
  {
    std::string namestr = split(name, ":")[0];
    std::string typestr = split(name, ":")[1];


    add_class(typestr);

    int typeIndex = _names[std::to_string(_names[typestr])];
    if (typeIndex == 0) {
      throw std::invalid_argument(std::string("name: ") +  typestr + "  NOT in Pool");
    }
    
    //int nameAndTypeIndex = _names[std::to_string(_names[namestr]) + "." + std::to_string(_names[ typestr ])];
    //if(nameAndTypeIndex == 0) {
    int nameAndTypeIndex;
      NameAndTypeHolder* ptr = new NameAndTypeHolder(_names[namestr],_names[typestr]);
      *this + ptr;
      nameAndTypeIndex = _names[ptr->get_params()];
      //}
    *this + new FieldRefHolder(class_index ,nameAndTypeIndex);

    return _names[std::to_string(class_index) + "." + std::to_string(nameAndTypeIndex)];
    
  }
  
  int ConstantPoolComponent::add_method_ref_index(const std::string& name)
  {
    validate_method_name(name);
    std::string classstr = split(name, '.')[0];
    std::string namestr = split(split(name, '.')[1],':')[0];
    std::string typestr = split(split(name, '.')[1],':')[1];

    add_class(classstr);
    int classIndex = _names[std::to_string(_names[classstr])];
    if (classIndex == 0) {
      throw std::invalid_argument(std::string("name: ") +  classstr + "  NOT in Pool");
    }
    if (_names.count(typestr) == 0 ) {
      add_name(typestr);
    }

    if (_names.count(namestr) == 0 ) {
      add_name(namestr);
    }

    int nameAndTypeIndex = _names[std::to_string(_names[namestr]) + "." + std::to_string(_names[ typestr ])];
    if(nameAndTypeIndex == 0) {
      NameAndTypeHolder* ptr = new NameAndTypeHolder(_names[namestr],_names[typestr]);
      *this + ptr;
      nameAndTypeIndex = _names[ptr->get_params()];
    }
    *this + new MethodRefHolder(classIndex,nameAndTypeIndex);
    return _names[std::to_string(classIndex) + "." + std::to_string(nameAndTypeIndex)];
  }
    
  void ConstantPoolComponent::display(std::ostream& out)
  {
    out << "CONST POOL COUNT = " << to_int(_bcount,2) << std::endl;
    out << "CONST POOL COUNT = " << _count  << std::endl;
    for(auto& x : _info) {
      out << x.index() << "  " << x.get_name() << "  " << x.get_params() << std::endl;
    }
  }

  void ConstantPoolComponent::load_from_buffer(const BYTE* buf)
  {
    memcpy(_bcount, buf, sizeof(_bcount));
    _count = bits_to_int<short>(_count, buf) - 1;
    int nextByte;
    nextByte = 2;
    bool skip = false;
    for(int idx = 1; idx < to_int(_bcount,sizeof(_bcount));idx++) {
      nextByte = add_info_from_raw(nextByte, buf,skip) + 1;
      if(skip) {
        ++idx;
        skip = false;
      }
    }
  }
    
  void ConstantPoolComponent::load_to_buffer(BYTE* buf) {
    memcpy(buf, _bcount, sizeof(_bcount)); 
    int idx = sizeof(_bcount);
    for (auto& x : _info) {
      x.load_to_buffer(buf + idx );
      idx += x.size_in_bytes();
    }
  }

  int ConstantPoolComponent::get_name_index(const std::string& tname)
  {
    return _names.count(tname) > 0 ? _names[tname] : 0;
  }
    
  int ConstantPoolComponent::add_name(const std::string& tname)
  {
    if (_names.count(tname) > 0) {
      return _names[tname];
    }
    // calc size in bytes
    int len = static_cast<int>(sizeof(PoolInfo::_tag) + sizeof(Utf8Holder::_length) + tname.length());
    // allocate buffer 
    BYTE* nbuf = new BYTE[len];
    //set tag
    nbuf[0] = TAG_Utf8;

    // set length
    short lenb = static_cast<short>(tname.length());
    write_big_e_bytes<short>(&nbuf[1], &lenb);

    // copy bytes for newname
    memcpy(&nbuf[3], tname.c_str(), lenb); 
    // incr count
    ++_count;
    short localbcount = to_int(_bcount,sizeof(_bcount))+1;
    write_big_e_bytes<short>(_bcount, &localbcount); 
    bool skip;
    add_info_from_raw(nbuf,skip);
    return _nextIndex;
  }
  
  std::string ConstantPoolComponent::get_name(const int idx) 
  {
    if(_ids.find(idx) == _ids.end()) throw std::invalid_argument("get_name: " + idx);
    return _ids[idx];
  }

}
