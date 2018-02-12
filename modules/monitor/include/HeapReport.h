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
//    along with French-Roast.  If not, see <http://www.gnu.org/licenses/>.
//

#ifndef HEAPREPORT_H
#define HEAPREPORT_H

namespace frenchroast { namespace monitor {
    class HeapReport {
      int         _min{0};
      int         _max{0};
      int         _current{0};
      bool        _first{true};
      std::string _class_name;
    public:
      HeapReport() = default;
      HeapReport& operator++();
      HeapReport& operator--();
      bool first();
      void set_classname(const std::string&);
      const std::string& classname() const;
      int current() const;
      int max() const;
      int min() const;
    };

  }
}
#endif
