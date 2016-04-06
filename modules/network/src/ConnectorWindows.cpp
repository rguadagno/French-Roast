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


#include "Connector.h"
#include <winsock2.h>
#include <windows.h>
#include <iostream>
#include <ws2tcpip.h>
#include <sstream>
#include <unordered_map>
#include <thread>


namespace frenchroast { namespace network {

    SOCKET _listener_socket;
    SOCKET _sender_socket;

    void Connector::init_receiver(const std::string& ipaddr, int port, void (*handler)(const std::string&))
    {
      _handler = handler;
      WSADATA wsaData;
      if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0 )
      {
        std::cout << "ERROR: "  <<  "STARTUP FAILED " << std::endl;
        exit(0);
      }

      _listener_socket = INVALID_SOCKET;
      _listener_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
      if ( _listener_socket == INVALID_SOCKET) {
        std::cout << "unable to create socket" << std::endl;
        WSACleanup();
        exit(0);
      }

      struct sockaddr_in connectInfo;
      connectInfo.sin_family      = AF_INET;
      connectInfo.sin_addr.s_addr = inet_addr(ipaddr.c_str());
      connectInfo.sin_port        = htons(port);
      int rv = bind(_listener_socket, (SOCKADDR*)&connectInfo, sizeof(connectInfo));
      if (rv == SOCKET_ERROR) {
        closesocket(_listener_socket);
        std::cout << "unable to bind to socket" << std::endl;
        WSACleanup();
        exit(0);
      }

      if (listen(_listener_socket, SOMAXCONN) == SOCKET_ERROR) {
        std::cout << "ERROR waiting..." << std::endl;
        WSACleanup();
        exit(0);
      }

      SOCKET inSock = accept(_listener_socket,NULL,NULL);
      std::thread t{[=]() {
        int rv;
        char databuf[500];
        memset(databuf,0,500);
        while(1) {
          rv = recv(inSock, databuf, 500, 0);
          if (rv < 0 ) {
            std::cout << "ERROR: " << WSAGetLastError() << std::endl;
            break;
          }
          std::string item{databuf};
          _handler(item);
        }
     }
    };
     t.detach();
    }


    void Connector::init_sender(const std::string& ipaddr, int port)
    {
      _sender_socket = INVALID_SOCKET;
      WSADATA wsaData;
      if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0 ) {
        std::cout << "ERROR: "  <<  "STARTUP FAILED " << std::endl;
        exit(0);
      }
      
      _sender_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
      if ( _sender_socket == INVALID_SOCKET) {
        std::cout << "unable to create socket" << std::endl;
        WSACleanup();
        exit(0);
      }

      struct sockaddr_in    connectInfo;
      connectInfo.sin_family      = AF_INET;
      connectInfo.sin_addr.s_addr = inet_addr(ipaddr.c_str());  // @@ fix this
      connectInfo.sin_port        = htons(port);
      int rv = connect(_sender_socket, (SOCKADDR*)&connectInfo, sizeof(connectInfo));
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
      closesocket(_listener_socket);
      WSACleanup();
    }
  }
}

