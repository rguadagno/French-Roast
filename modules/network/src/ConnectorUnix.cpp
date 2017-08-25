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

#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#include <iostream>
#include <sstream>
#include <cstring>
#include <unordered_map>
#include "Connector.h"
#include "Listener.h"

namespace frenchroast { namespace network {

    int Connector::get_pid()
    {
      return getpid();
    }
    
std::string Connector::get_hostname()
    {
      char buf[100];
      gethostname(buf, 100);
      return std::string{buf};
    }
    
    void Connector::wait_for_client_connection(const std::string& ipaddr, int port, Listener* handler, bool silent)
    {
      _handler = handler;
      _receiver_socket = socket(AF_INET, SOCK_STREAM, 0);
      struct sockaddr_in serverinfo;
      memset(&serverinfo, 0, sizeof(serverinfo));
      serverinfo.sin_family = AF_INET;
      serverinfo.sin_addr.s_addr = inet_addr(ipaddr.c_str());
      serverinfo.sin_port = htons(port);
      bind(_receiver_socket, (struct sockaddr *)&serverinfo, sizeof(serverinfo));

      listen(_receiver_socket, 10);

      if(!silent)
        std::cout << "waiting..." << std::flush;
      struct sockaddr_in conn_from;
      unsigned int len = sizeof(conn_from);
      while(1) {
        int connfd = accept(_receiver_socket, (struct sockaddr *) &conn_from, &len);
        std::string ipport = std::string{inet_ntoa(conn_from.sin_addr)} + ":" + std::to_string(htons(conn_from.sin_port));
        _ipport_sendersocket[ipport] = connfd;
        if(!silent)
          std::cout << "connected."<< ipport << std::endl;
        _handler->message(ipport + "~connected");
        std::thread t1{process_instream,connfd,ipport, _handler};
        t1.detach();
      }
    }

    void Connector::connect_to_server(const std::string& ipaddr, int port, Listener* handler)
    {
      _sender_socket = socket(AF_INET, SOCK_STREAM, 0);
      _ipport_sendersocket["client"] =  _sender_socket;
      struct sockaddr_in serverinfo;
      memset(&serverinfo, 0, sizeof(serverinfo));
      serverinfo.sin_family = AF_INET;
      serverinfo.sin_addr.s_addr = inet_addr(ipaddr.c_str());
      serverinfo.sin_port = htons(port);
      int rv = connect(_sender_socket, (struct sockaddr *)&serverinfo, sizeof(serverinfo));
      if (rv != 0) {
        std::cout << "========== Unable to connect to server ( "<< ipaddr << ")=== " << serverinfo.sin_addr.s_addr << "  " << serverinfo.sin_port << std::endl;
        std::cout << errno << std::endl;
        exit(0);
      }
      std::cout << "========== CONNECTED TO SERVER ============ " << serverinfo.sin_addr.s_addr << std::endl;
      if(handler != nullptr) {
        std::thread t1{process_instream,_sender_socket,"",handler};
        t1.detach();
      }

    }

    void Connector::blocked_listen(Listener* handler)
    {
      process_instream(_sender_socket,"", handler);
    }

    void Connector::send_message(const std::string& msg)
    {
      for(auto& s : _ipport_sendersocket) {
        send(s.second, msg.c_str(), msg.length()+1,0);
      }
    }

    void Connector::send_message(const std::string& ipport, const std::string& msg)
    {
      send(_ipport_sendersocket[ipport], msg.c_str(), msg.length()+1,0);
    }

    void Connector::close_down()
    {
      close(_sender_socket);
    }
  }
}

