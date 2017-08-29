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
#include <unordered_map>
#include "Listener.h"
#include "FRSocket.h"

namespace frenchroast { namespace network {
    

    void process_instream(std::string, Listener* handler,std::unordered_map<std::string, FRSocket>&);
      
    class Connector {
      FRSocket  _receiver_socket;
      std::unordered_map<std::string, FRSocket> _ipport_sendersocket;
      Listener* _handler;
   
    public:
      void wait_for_client_connection(const std::string& ipaddr, int port, Listener* handler, bool silent=true);
      void connect_to_server(const std::string& ipaddr, int port, Listener* handler = nullptr);
      void blocked_listen(Listener*);
      void send_message(const std::string& msg);
      void send_message(const std::string& ip_port, const std::string& msg);
      void close_down();
      static std::string get_hostname();
      static int get_pid();
    };
  }
}

#endif
