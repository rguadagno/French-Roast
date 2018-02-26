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
      const  char SIGNAL           = 'A';
      const  char SIGNAL_TIMER     = 'B';
      const  char TRAFFIC          = 'C';
      const  char JAMMED           = 'D';
      const  char LOADED           = 'E';
      const  char READY            = 'F';
      const  char CONNECTED        = 'G';
      const  char UNLOADED         = 'H';
      const  char ACK_PROFILER_ON  = 'I';
      const  char ACK_PROFILER_OFF = 'J';
      const  char TRANSMIT_OPCODES = 'K';
      const  char TRANSMIT_HOOKS   = 'L';
      const  char HEAP_EVENT       = 'M';
      const  char SIGNAL_REPORTS   = 'N';
      const  char METHOD_STATS     = 'O';
      const  char INSTRUMENTATION  = 'P';
    }
  }
}
#endif
