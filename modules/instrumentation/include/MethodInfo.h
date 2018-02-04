// copyright (c) 2018 Richard Guadagno
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
#ifndef FRMETHODINFO_H
#define FRMETHODINFO_H
#include <iostream>
#include <unordered_map>
#include <vector>
#include "Attribute.h"
#include "Instruction.h"
#include "Exception.h"
#include "StackMapFrame.h"

namespace frenchroast {

  class MethodInfo {
    short _access_flags{0};
    short _name_index{0};
    short _descriptor_index{0};
    short _attributes_count{0};

    short _max_stack{0};
    short _max_locals{0};
    int   _code_length{0};
    short _exception_table_length{0};
    short _code_attributes_count{0};
    std::vector<Exception> _exceptions;

    std::unordered_map<std::string, Attribute<std::string>> _attributes;
    std::unordered_map<std::string, Attribute<std::string>> _codeAttributes;  
    std::vector<Instruction> _instructions;
    std::vector<StackMapFrame*> _frames;


    
    std::vector<int> get_targets(Instruction& inst);
    void update_targets(Instruction& inst, std::unordered_map<int,int> origAddr, std::unordered_map<int,int> newAddr, std::vector<int>& origTargets, int& nextTarget);
    void update_exception(Exception& excep, std::unordered_map<int,int> origAddr, std::unordered_map<int,int> newAddr);
    void adjust_frames(std::vector<StackMapFrame*>& frames);
    void load_attributes_from_buffer(BYTE*&, short count, std::unordered_map<std::string, Attribute<std::string>>& dest, std::unordered_map<int,std::string>& names);
    void load_attributes_to_buffer(BYTE*& ptr, std::unordered_map<std::string, Attribute<std::string>>& src);

  public:
    
    MethodInfo() = default;
    MethodInfo(const MethodInfo&);
    MethodInfo& operator=(MethodInfo&&);
    MethodInfo(short flags,short name_index,short descriptor_index);
    ~MethodInfo();
    int load_from_buffer(const BYTE* buf,  std::unordered_map<int,std::string>& names);
    int load_to_buffer(BYTE* buf, std::unordered_map<std::string,int>& ids);

    std::vector<int> get_return_addresses() const;
    short get_max_stack() const;
    void set_max_stack(short);
    void add_instructions(int insertAt,  std::vector<Instruction>&, bool sticky);
    Instruction& operator[](int idx);
    int size_in_bytes() const;
    std::vector<StackMapFrame*> frames();
    void set_name_index(short);
    void set_descriptor_index(short);
    void set_access_flags(short);
    short get_name_index() const;
    short get_descriptor_index() const;


  };

}
#endif
