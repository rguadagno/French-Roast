// copyright (c) 2016 Richard Guadagno
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

#ifndef FRCMD_H
#define FRCMD_H


namespace frenchroast { namespace agent {

    class CommandListener {
    public:
      virtual void watch_traffic(const int interval_millis) = 0;
      virtual void stop_watch_traffic() = 0;
      virtual void watch_loading() = 0;
      virtual void stop_watch_loading() = 0;
      virtual void turn_on_profiler() = 0;
      virtual void turn_off_profiler() = 0;

    };
  }
}

#endif
