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
//    along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
//

#ifndef FRCON_H
#define FRCON_H
#include <string>

namespace frenchroast { namespace network {
    class Connector {
      void (*_handler)(const std::string&);
    public:
      void init_receiver(const std::string& ipaddr, int port, void (*handler)(const std::string&));
      void init_sender(const std::string& ipaddr, int port);
      void send_message(const std::string& msg);
      void close_down();
    };
  }
}

#endif
