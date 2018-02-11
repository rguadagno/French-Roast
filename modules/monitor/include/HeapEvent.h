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

#ifndef HEAPEVENT_H
#define HEAPEVENT_H

#include <string>
#include "Command.h"

namespace frenchroast { namespace monitor {
    class HeapEvent  {
      friend HeapEvent& operator>>(const std::string& rep, HeapEvent& ref);
      std::string   _class_name{""};
      long long     _tag;
      bool          _free_event{false};
    public:

      HeapEvent(const char* classsname, long long tag);
      HeapEvent(long long tag);
      HeapEvent() = default;;
      const std::string& classname() const;
      long long tag() const;
      bool is_free() const;
    };

    template <typename OutType>
      OutType& operator<<(OutType& out, const HeapEvent& ref)
      {
        out << command::HEAP_EVENT << "~" << (ref.is_free() ? "free" : "create") << "<end-type>" << ref.tag() << "<end-tag>" << ref.classname() << "<end-classname>";
        return out;
      }

    HeapEvent& operator>>(const std::string& rep, HeapEvent& ref);
    HeapEvent& operator>>(const std::string& rep, HeapEvent&& ref);
  }
}
#endif
