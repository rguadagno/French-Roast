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
#include "Exception.h"

namespace frenchroast {
  
  void Exception::load_from_buffer(const BYTE* buf)
  {
    _startPC   = to_int(buf,     2);
    _endPC     = to_int(buf + 2 ,2);
    _handlerPC = to_int(buf + 4 ,2);
    _catchType = to_int(buf + 6 ,2);
  }

  void Exception::load_to_buffer(BYTE* buf)
  {
    write_bytes(buf,     _startPC, 2);
    write_bytes(buf + 2, _endPC, 2);
    write_bytes(buf + 4, _handlerPC, 2);
    write_bytes(buf + 6, _catchType, 2);
  }


  short Exception::get_start_pc()
  {
    return _startPC;
  }

  short Exception::get_end_pc()
  {
    return _endPC;
  }
  
  short Exception::get_handler_pc()
  {
    return _handlerPC;
  }
  
  void Exception::set_start_pc(short addr)
  {
    _startPC = addr;
  }
  
  void Exception::set_end_pc(short addr)
  {
    _endPC = addr;
  }
  
  void Exception::set_handler_pc(short addr)
  {
    _handlerPC = addr;
  }
  

  
}
