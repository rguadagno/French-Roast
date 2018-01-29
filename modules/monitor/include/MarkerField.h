// copyright (c) 2017 Richard Guadagno
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

#ifndef MARKERFIELD_H
#define MARKERFIELD_H

#include "SignalParams.h"
#include "SignalMarkers.h"

namespace frenchroast { namespace monitor {
    class MarkerField {
    public:
      MarkerField(std::string desc, int count = 1) : _descriptor(desc), _count(count) {}
    MarkerField(const std::string& skey,const SignalParams& params, const SignalMarkers& markers) : _descriptor(skey),_count(1),_arg_items(params), _instance_items(markers) {}
    MarkerField() {}
      std::string _descriptor;
      int         _count{-1};
      SignalParams _arg_items;
      SignalMarkers _instance_items;
      MarkerField& operator++() { ++_count; return *this; }
      MarkerField& operator+=(const MarkerField& ref)
        {
          if(_count == -1) {
            _count = 1;
            _arg_items = ref._arg_items;
            _instance_items = ref._instance_items;            
          }
          else {
            ++_count;
          }
          return *this;
        }
    };
  }
}


#endif
