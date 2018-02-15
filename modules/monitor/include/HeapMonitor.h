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

#ifndef HEAPMON_H
#define HEAPMON_H

#include "HeapEvent.h"
#include "HeapReport.h"

namespace frenchroast { namespace monitor {
    template
      <typename HandlerType>
      class HeapMonitor {
      HandlerType&  _handler;
      std::unordered_map<long long, HeapReport*>  _tag_lookup;
      std::unordered_map<std::string, HeapReport> _reports;
    public:
      HeapMonitor(HandlerType& handler) : _handler(handler)
      {
      }

      void process(HeapEvent hevent)
      {
        if(hevent.is_free()) {
          _handler.heap_event(*(_tag_lookup[hevent.tag()])-= hevent);
        }
        else {
          HeapReport& rpt = _reports[hevent.classname()];
          if(rpt.first()) {
            rpt.set_classname(hevent.classname());
          }

          _tag_lookup[hevent.tag()] = &_reports[hevent.classname()];
          rpt += hevent;
          _handler.heap_event(rpt += hevent);
        }
      }
      void reset() {
        _reports.clear();
        _tag_lookup.clear();
      }
    };



  }
}
#endif
