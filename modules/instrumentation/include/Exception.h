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
#ifndef EXCEP_H
#define EXCEP_H

#include "Util.h"

namespace frenchroast {
  class Exception {
    short _startPC;
    short _endPC;
    short _handlerPC;
    short _catchType;
  public:
    void load_from_buffer(const BYTE* buf);
    void load_to_buffer(BYTE* buf);
    short get_start_pc();
    short get_end_pc();
    short get_handler_pc();
    void set_start_pc(short addr);
    void set_end_pc(short addr);
    void set_handler_pc(short addr);
    

  };
}
#endif
