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

#ifndef FRCON_H
#define FRCON_H
#include <string>
#include "Listener.h"

namespace frenchroast { namespace network {

    class Connector {
#ifdef CONNECTOR_UNIX
    int  _receiver_socket;
    int  _sender_socket;
#else
    SOCKET _listener_socket;
    SOCKET _sender_socket;
#endif

      Listener* _handler;
    public:
      void init_receiver(const std::string& ipaddr, int port, Listener* handler);
      void init_sender(const std::string& ipaddr, int port);
      void send_message(const std::string& msg);
      void close_down();
    };
  }
}

#endif
