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

namespace frenchroast { namespace monitor {
    class MarkerField {
    public:
      MarkerField(std::string desc, int count = 1) : _descriptor(desc), _count(count) {}
    MarkerField() : _count(-1) {}
      std::string _descriptor;
      int         _count;
      std::vector<std::string> _arg_items;
      std::vector<std::string> _instance_items;
      MarkerField& operator++() { ++_count; return *this; }
    };
  }
}


#endif
