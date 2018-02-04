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

#ifndef CP_H
#define CP_H


#include "ClassFileComponent.h"

namespace frenchroast {
    
  static const unsigned char TAG_utf8               = 1;
  static const unsigned char TAG_Class              = 7;
  static const unsigned char TAG_FieldRef           = 9;
  static const unsigned char TAG_MethodRef          = 10;
  static const unsigned char TAG_InterfaceMethodRef = 11;
  static const unsigned char TAG_String             = 8;
  static const unsigned char TAG_Integer            = 3;
  static const unsigned char TAG_Float              = 4;
  static const unsigned char TAG_Long               = 5;
  static const unsigned char TAG_Double             = 6;
  static const unsigned char TAG_NameAndType        = 12;
  static const unsigned char TAG_Utf8               = 1;
  static const unsigned char TAG_MethodHandle       = 15;
  static const unsigned char TAG_MethodType         = 16;
  static const unsigned char TAG_InvokeDynamic      = 18;

  class InfoHolder {
  protected:
    std::string _name="MISSINGNAME";
    
  protected:
    BYTE _tag;

  public:
    const int TAGSIZE=1;
    virtual std::string get_params() const = 0;
    virtual ~InfoHolder() {}
    virtual int size_in_bytes() const =0;
    std::string get_name() const
    {
      return _name;
    }
    virtual void load_to_buffer(BYTE* buf)=0;
    BYTE get_tag() const
    {
      return _tag;
    }
  };

  class IntegerHolder : public InfoHolder {
    BYTE              _bytes[4];
  public:
    IntegerHolder(int value);
    IntegerHolder(const BYTE* buf);
    int         size_in_bytes() const;
    std::string get_params() const;
    void        load_to_buffer(BYTE* buf);
  };
 
  class PoolInfo {

    InfoHolder*   _holder;
    //--------------------
    int           _index; 
    
    PoolInfo(const PoolInfo& ref);
  public:
    BYTE          _tag;
    PoolInfo(unsigned char tag, int idx, InfoHolder* holder);
    PoolInfo(PoolInfo&& ref);
    int get_tag();
    std::string get_name() const
    {
      return _holder->get_name();
    }
    std::string get_params() const;
    ~PoolInfo();
    int size_in_bytes() const;
    void load_to_buffer(BYTE* buf);
    int index() const;
  };

  class ConstantPoolComponent : public ClassFileComponent {
    BYTE _bcount[2];
    short _count;       
    int _nextIndex;
    std::vector<PoolInfo>               _info;
    std::unordered_map<std::string,int> _names;
    std::unordered_map<int,std::string> _ids;
    InfoHolder* build_holder(unsigned char tag, const unsigned char* buf);
  public:
    ConstantPoolComponent() {_nextIndex = 0; }
    int add_info_from_raw(int idx, const unsigned char* buf,bool& skip);
    int add_info_from_raw(unsigned char* buf,bool& skip);
    int add_info(BYTE tag, InfoHolder* item,bool& skip);
    int get_name_index(const std::string& tname);
    int add_name(const std::string& tname);
    int add_string(const std::string& tname);
    int add_class(const std::string& tname);
    int add_class(int nameid);
    void display(std::ostream& out);
    void write_bytes(std::ostream& out) const;
    int size_in_bytes() const;
    void reset();
    void load_from_buffer(const BYTE* buf);
    void load_to_buffer(BYTE* buf);
    std::string resolve_name(int id);
    int  next_index() const;
    int operator+(InfoHolder* ref);
    int get_method_ref_index(const std::string& name);
    int get_name_and_type_index(const std::string& name);
    int add_method_ref_index(const std::string& name);
    int add_field_ref_index(int class_id,const std::string& name);
    std::string get_name( int idx) ;
    static void validate_method_name(const std::string& name);
    std::unordered_map<int,std::string> get_ids() const;
    std::unordered_map<std::string,int> get_names() const;
  };
}
#endif
