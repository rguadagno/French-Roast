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

#ifndef MONITORCMD_H
#define MONITORCMD_H

namespace frenchroast { namespace monitor { namespace command {
      const std::string SIGNAL           = "signal";
      const std::string SIGNAL_TIMER     = "signaltimer";
      const std::string TRAFFIC          = "traffic";
      const std::string JAMMED           = "jammed";
      const std::string LOADED           = "loaded";
      const std::string READY            = "ready";
      const std::string CONNECTED        = "connected";
      const std::string UNLOADED         = "unloaded";
      const std::string ACK_PROFILER_ON  = "ack_profiler_on";
      const std::string ACK_PROFILER_OFF = "ack_profiler_off";
      const std::string TRANSMIT_OPCODES = "transmit-opcodes";
      const std::string TRANSMIT_HOOKS   = "transmit-hooks";
      const std::string HEAP_EVENT       = "heap-event";
    }
  }
}
#endif
