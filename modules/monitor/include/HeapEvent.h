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
#include <sstream>
#include "Command.h"
#include "StackReport.h"

namespace frenchroast { namespace monitor {
    class HeapEvent  {
      friend HeapEvent& operator>>(const std::string& rep, HeapEvent& ref);
      std::string   _class_name{""};
      long long     _tag;
      bool          _free_event{false};
      StackReport    _srpt;
    public:

      HeapEvent(const char* classsname, long long tag, const StackReport&);
      explicit HeapEvent(long long tag);
      HeapEvent() = default;;
      const std::string& classname() const;
      long long          tag() const;
      bool               is_free() const;
      const StackReport& report() const;
    };

    std::stringstream& operator<<(std::stringstream& out,  const HeapEvent& ref);
    HeapEvent& operator>>(const std::string& rep, HeapEvent& ref);
    HeapEvent& operator>>(const std::string& rep, HeapEvent&& ref);
  }
}
#endif
