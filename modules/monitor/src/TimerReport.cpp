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


#include <iostream>
#include "TimerReport.h"
#include "Util.h"

namespace frenchroast { namespace monitor {


    TimerReport::TimerReport(const std::string& tname, const Descriptor& dsc, long long millis, const std::string& direction) :
      _thread_name(tname),
      _descriptor(dsc),
      _time_in_millis(millis)
    {
      _direction = (direction == "enter" ?  Direction::Enter : Direction::Exit);
      _key = tname + _descriptor.full_name();
    }
    
    long TimerReport::elapsed() const
    {
      return _elapsed;
    }
    
    const std::string& TimerReport::key() const
    {
      return _key;
    }
    
    const std::string& TimerReport::thread_name() const
    {
      return _thread_name;
    }

    const Descriptor& TimerReport::descriptor() const
    {
      return _descriptor;
    }

    std::string TimerReport::descriptor_name() const
    {
      return _descriptor.full_name();
    }

    TimerReport& TimerReport::operator+=(const TimerReport& ref)
    {
      _elapsed += ref._time_in_millis - _time_in_millis;
      return *this;
    }

    long long TimerReport::time_in_millis() const
    {
      return _time_in_millis;
    }
    
    TimerReport::Direction TimerReport::direction() const
    {
      return _direction;
    }

    TimerReport& TimerReport::operator=(const TimerReport& ref)
    {
      _time_in_millis = ref._time_in_millis;
      _thread_name = ref._thread_name;
      _descriptor = ref._descriptor;
     
      return *this;
    }
    
    TimerReport& operator>>(const std::string& rep, TimerReport& ref)
    {
      ref._thread_name = frenchroast::split(rep, "<end-thread-name>")[0];
      frenchroast::split(frenchroast::split(rep, "<end-descriptor>")[0], "<end-thread-name>")[1] >> ref._descriptor;
      ref._elapsed = std::stoll(   frenchroast::split(frenchroast::split(rep, "<end-elapsed>")[0], "<end-descriptor>")[1]);
      ref._time_in_millis = std::stoll(   frenchroast::split(frenchroast::split(rep, "<end-time>")[0], "<end-elapsed>")[1]);
      ref._key = ref._thread_name + ref._descriptor.full_name();
      std::string dir = frenchroast::split(rep, "<end-time>")[1];
      ref._direction = (dir == "enter" ? TimerReport::Direction::Enter : TimerReport::Direction::Exit);
      return ref;
    }
    
  }
}
