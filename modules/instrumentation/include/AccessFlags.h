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

#ifndef ACCESS_FLAGS_H
#define ACCESS_FLAGS_H
#include "ClassFileComponent.h"

namespace frenchroast {
    
  class AccessFlagsComponent : public ClassFileComponent {
    BYTE _flags[2];
    std::string getFlagString(int flags);
  public:
    int  size_in_bytes() const;
    void reset();
    void load_from_buffer(const BYTE* buf);
    void load_to_buffer(BYTE* buf);
    void display(std::ostream& out);
    void set(int flag);
    void remove(int flag);
    static const int ACC_PUBLIC     = 0x0001;
    static const int ACC_FINAL      = 0x0010;
    static const int ACC_SUPER      = 0x0020;
    static const int ACC_INTERFACE  = 0x0200;
    static const int ACC_ABSTRACT   = 0x0400;
    static const int ACC_SYNTHETIC  = 0x1000;
    static const int ACC_ANNOTATion = 0x2000;
    static const int ACC_ENUM       = 0x4000;

    static const int ACC_PRIVATE      = 0x0002;
    static const int ACC_PROTECTED    = 0x0004;
    static const int ACC_STATIC       = 0x0008;
    static const int ACC_SYNCHRONIZED = 0x0020;
    static const int ACC_BRIDGE       = 0x0040;
    static const int ACC_VARARGS      = 0x0080;
    static const int ACC_NATIVE       = 0x0100;
    static const int ACC_STRICT       = 0x0800;
  };
}
#endif
