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

#ifndef SM_H
#define SM_H
#include <vector>
#include "ClassFileComponent.h"
#include "Util.h"

namespace frenchroast {

  class StackMapFrame {
  protected:
    BYTE  _frameType;
  public:
    virtual int size_in_bytes() const = 0;
    virtual  void adjust_offset(int amount) = 0;
    virtual void load_from_buffer(BYTE* buf) = 0;
    virtual void load_to_buffer(BYTE* buf) = 0;
  };

  std::vector<StackMapFrame*> load_frames_from_buffer(int count,BYTE* buf); 
  void load_frames_to_buffer(std::vector<StackMapFrame*> items, BYTE* buf);
 
}
#endif
