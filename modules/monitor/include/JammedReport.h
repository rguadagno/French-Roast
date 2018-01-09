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

#ifndef JAMMEDH_H
#define JAMMEDH_H

#include <unordered_set>
#include <unordered_map>
#include "StackTrace.h"

namespace frenchroast { namespace monitor {
    class JammedReport  {
      friend JammedReport& operator>>(const std::string& rep, JammedReport& ref);
      std::string              _key;
      StackTrace               _waiter;
      StackTrace               _owner;
      std::unordered_set<std::string>    _monitors;
    public:

      JammedReport(StackTrace waiter, StackTrace owner);
      JammedReport();
      std::vector<std::string> monitors() const;
      JammedReport& add_monitor(const std::string&, bool xform=true);
      const std::string& key() const;
      const StackTrace& waiter() const;
      const StackTrace& owner() const;
      bool operator==(const JammedReport&) const;
      JammedReport& operator+=(const JammedReport&);
      const static std::string TAG_END;
      const static std::string TAG;
    };

    template <typename OutType>
      OutType& operator<<(OutType& out, const JammedReport& ref)
      {

        for(auto x : ref.monitors()) {
          out << x << "<end-monitor>";
        }
        out  << "<end-jmonitors>";
        out << ref.owner() << "<end-owner>" << ref.waiter(); 
        return out;
      }

    template <typename OutType>
      OutType& operator<<(OutType& out, const std::unordered_map<std::string,JammedReport>& ref)
      {
        if(ref.size() == 0) return out;
        out << JammedReport::TAG << "<view>";        
        for(auto x : ref) {
          out << x.second << JammedReport::TAG_END;
        }
        return out;
      }

    
    JammedReport& operator>>(const std::string& rep, JammedReport& ref);
    std::vector<JammedReport>& operator>>(const std::string&, std::vector<JammedReport>& ref);

  }
}
#endif
