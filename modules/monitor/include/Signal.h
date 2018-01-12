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

#ifndef SIGNAL_H
#define SIGNAL_H

#include <unordered_set>
#include <unordered_map>
#include "StackReport.h"
#include "SignalParams.h"
#include "SignalMarkers.h"


namespace frenchroast { namespace monitor {
    class Signal  {

      StackReport    _report;
      SignalParams    _params;
      SignalMarkers   _markers;            
        
    public:
      Signal(const StackReport&, const SignalParams&, const SignalMarkers);

      const std::string thread_name() const;
      const std::string key() const;
      const std::size_t count() const;
      const std::string descriptor() const;
      const SignalParams& params() const;
      const SignalMarkers& markers() const;
      //      const static std::string TAG_END;
      //const static std::string TAG;
    };
    /*
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
    */
  }
}
#endif
