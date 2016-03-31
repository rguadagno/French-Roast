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
#ifndef OPCODE_CONST_H
#define OPCODE_CONST_H

namespace frenchroast { namespace opcode {
    const BYTE bipush        = 16;
    const BYTE ldc           = 18;
    const BYTE iload_0       = 26;
    const BYTE iload_1       = 27;
    const BYTE aload_0       = 42;
    const BYTE if_icmpge     = 162;
    const BYTE xgoto         = 167;
    const BYTE tableswitch   = 170;
    const BYTE lookupswitch  = 171;
    const BYTE xreturn       = 177;
    const BYTE getstatic     = 178;
    const BYTE invokevirtual = 182;
    const BYTE invokestatic  = 184;
  }
}
#endif