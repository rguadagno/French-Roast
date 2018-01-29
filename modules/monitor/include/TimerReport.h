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

#ifndef TIMERRPT_H
#define TIMERRPT_H

#include "Signal.h"
#include "MarkerField.h"


namespace frenchroast { namespace monitor {

    class TimerReport  {
      enum class Direction {Enter, Exit};
      friend TimerReport& operator>>(const std::string&, TimerReport& ref);
      Descriptor                                     _descriptor;
      std::string                                    _thread_name;
      long                                           _elapsed{0};
      long long                                      _time_in_millis{0};
      std::string                                    _key{""};
      Direction                                      _direction;
    public:
      TimerReport()=default;
      TimerReport(const std::string&, const Descriptor&, long long millis, const std::string& direction);
      long                 elapsed() const;
      const std::string&   key() const;
      const std::string&   thread_name() const;
      const Descriptor&    descriptor() const;
      std::string          descriptor_name() const;
      Direction            direction() const;
      TimerReport&         operator+=(const TimerReport&);
      TimerReport&         operator=(const TimerReport&);
      long long            time_in_millis() const;
    };

    template <typename OutType>
      OutType& operator<<(OutType& out, const TimerReport& ref)
      {
        out << ref.thread_name() << "<end-thread-name>" << ref.descriptor() << "<end-descriptor>" << ref.elapsed() << "<end-elapsed>" << ref.time_in_millis() << "<end-time>" << (ref.direction() == TimerReport::Direction::Enter ? "enter" : "exit");
        return out;
      }

    template <typename OutType>
      OutType& operator<<(OutType& out, const std::unordered_map<std::string, TimerReport>& timers)
      {
        if(timers.size() == 0) return out;
        for(auto& rpt : timers) {
          out << rpt.second <<  "<end-timer-report>";
        }
        return out;
      }

  
   TimerReport& operator>>(const std::string&, TimerReport& ref);
   std::vector<TimerReport>& operator>>(const std::string&, std::vector<TimerReport>& ref);

    
    
  }
}
#endif
