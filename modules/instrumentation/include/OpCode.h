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

#ifndef OP_H
#define OP_H
#include <unordered_map>
#include <bitset>
#include <regex>
#include "ClassFileComponent.h"

namespace frenchroast {
  class OpCode {
    BYTE           _code;
    int            _size{0};;
    std::string    _name;
    std::bitset<4> _attributes;
    bool           _isDynamic;
    static std::unordered_map<BYTE, OpCode> _op_codes;
    const static std::regex  _lineRegex;//{"[a-z0-9]+[\b]*<[0-9]*[\b]*[1]"};
    //    std::regex  _fullRegex{"((?:[A-Za-z0-9_]+\\.)+[A-Za-z0-9]+)::([A-Za-z0-9_\\*]+(?::\\([a-z]*\\):[a-z]+){0,})(<ENTER>|<EXIT>|<TIMER>){1}((?:\\[[A-Za-z0-9_]+\\]){0,})((?:<ARTIFACTS:OFF>){0,})"};
  public:
    const static std::bitset<4> None;
    const static std::bitset<4> Branch;
    const static std::bitset<4> Raw;
    OpCode(BYTE code, int size,const std::string& name, const std::bitset<4> attribute = None);
    OpCode(BYTE code, int size,const std::string& name, const std::bitset<4> attribute, bool isdynamic);
    OpCode();
    OpCode& operator[](BYTE op);
    operator BYTE() const;
    bool is_branch() const;
    bool is_raw() const;
    int get_size() const;
    bool is_dynamic() const;
    std::string get_name() const;
    BYTE get_code() const;
    static void load(std::string);
    static void load_from_file(const std::string&);
  };
}


#endif
