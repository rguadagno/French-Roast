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

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "Connector.h"
#include <iostream>
#include <sstream>
#include <cstring>
#include <sstream>
#include <unordered_map>


#include "Listener.h"

namespace frenchroast { namespace network {
    std::string ntoa(int x) 
    {
      std::stringstream ss;
      ss << x;
      return ss.str();
    }
    void Connector::init_receiver(const std::string& ipaddr, int port, Listener* handler)
    {
      _handler = handler;
      _receiver_socket = socket(AF_INET, SOCK_STREAM, 0);
      struct sockaddr_in serverinfo;
      memset(&serverinfo, 0, sizeof(serverinfo));
      serverinfo.sin_family = AF_INET;
      serverinfo.sin_addr.s_addr = inet_addr(ipaddr.c_str());
      serverinfo.sin_port = htonl(port);
      bind(_receiver_socket, (struct sockaddr *)&serverinfo, sizeof(serverinfo));

      listen(_receiver_socket, 1);

      std::cout << "waiting..." << std::flush;
      struct sockaddr_in conn_from;
      unsigned int len = sizeof(conn_from);
      int connfd = accept(_receiver_socket, (struct sockaddr *) &conn_from, &len);
      std::cout << "connected." << std::endl;

      _handler->message(std::string("connected~") + inet_ntoa(conn_from.sin_addr) + ":" + ntoa(htons(conn_from.sin_port)));
      int rv;
      char databuf[500];
      char flowbuf[1000];
      char strbuf[500];
      int start = 0;
      int end = 0;
      int buflen = 0;

      memset(databuf,0,500);
      while(1) {
        rv = recv(connfd, databuf, 500, 0);
        if (rv < 0 ) {
          std::cout << "RECV ERROR: " << std::endl;
          break;
        }

	memcpy(&flowbuf[buflen], databuf, rv);
	int total_bytes = buflen + rv;

	while(end < total_bytes) {
	  if (flowbuf[end] == '\0') {
	    memcpy(strbuf, &flowbuf[start],(end-start) + 1);
	    std::string item{strbuf};
	    _handler->message(item);
	    start = end + 1;
	}
	++end;
      }
      buflen = end-start;
      if(buflen > 0) {
	memcpy(flowbuf, &flowbuf[end], buflen);
      }
      end = 0;
      start = 0;
      }
    }

    void Connector::init_sender(const std::string& ipaddr, int port)
    {
      _sender_socket = socket(AF_INET, SOCK_STREAM, 0);
      struct sockaddr_in serverinfo;
      memset(&serverinfo, 0, sizeof(serverinfo));
      serverinfo.sin_family = AF_INET;
      serverinfo.sin_addr.s_addr = inet_addr(ipaddr.c_str());
      serverinfo.sin_port = htons(port);
      int rv = connect(_sender_socket, (struct sockaddr *)&serverinfo, sizeof(serverinfo));
      if (rv != 0) {
        std::cout << "========== Unable to connect to server ============ " << std::endl;
        exit(0);
      }
      std::cout << "========== CONNECTED TO SERVER ============ " << std::endl;
    }


    void Connector::send_message(const std::string& msg)
    {
      send(_sender_socket, msg.c_str(), msg.length()+1,0);
    }


    void Connector::close_down()
    {
      //
    }
  }
}

