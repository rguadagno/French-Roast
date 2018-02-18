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
#include "Command.h"


namespace frenchroast { namespace monitor {
    class Signal  {
      friend     Signal& operator>>(const std::string&, Signal& ref);
      
      StackReport              _report;
      SignalParams             _params;
      SignalMarkers            _markers;            

    public:
      Signal(const StackReport&, const SignalParams&, const SignalMarkers);
      Signal()=default;
      const std::string        thread_name() const;
      const std::string        key() const;
      const std::string        descriptor() const;
      const StackReport&       report() const;
      const SignalParams&      params() const;
      const SignalMarkers&     markers() const;
      bool                     operator==(const Signal&) const;
    };

    template <typename OutType>
      OutType& operator<<(OutType& out, const Signal& ref)
      {
        out << command::SIGNAL << ref.report() << "<end-report>" << ref.params() << "<end-params>" << ref.markers();
        return out;
      }

      Signal& operator>>(const std::string&, Signal& ref);
      Signal& operator>>(const std::string&, Signal&& ref);
      std::vector<Signal>& operator>>(const std::string& rep, std::vector<Signal>& ref);    
  }
}
#endif
