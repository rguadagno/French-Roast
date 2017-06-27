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



#include <winsock2.h>
#include <windows.h>
#include <iostream>
#include <ws2tcpip.h>
#include <sstream>
#include <unordered_map>
#include <thread>
#include "Connector.h"
#include "Util.h"

#include "Listener.h"

namespace frenchroast { namespace network {

    void Connector::wait_for_client_connection(const std::string& ipaddr, int port, Listener* handler, bool silent)
    {
      _handler = handler;
      WSADATA wsaData;
      if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0 )
      {
        std::cout << "ERROR: "  <<  "STARTUP FAILED " << std::endl;
        exit(0);
      }

      _receiver_socket = INVALID_SOCKET;
      _receiver_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
      if ( _receiver_socket == INVALID_SOCKET) {
        std::cout << "unable to create socket" << std::endl;
        WSACleanup();
        exit(0);
      }

      struct sockaddr_in connectInfo;
      connectInfo.sin_family      = AF_INET;
      connectInfo.sin_addr.s_addr = inet_addr(ipaddr.c_str());
      connectInfo.sin_port        = htons(port);
      int rv = bind(_receiver_socket, (SOCKADDR*)&connectInfo, sizeof(connectInfo));
      if (rv == SOCKET_ERROR) {
        closesocket(_receiver_socket);
        std::cout << "unable to bind to socket" << std::endl;
        WSACleanup();
        exit(0);
      }

      if (listen(_receiver_socket, SOMAXCONN) == SOCKET_ERROR) {
        std::cout << "ERROR waiting..." << std::endl;
        WSACleanup();
        exit(0);
      }

      struct sockaddr_in    conn_from;
      int len = sizeof(conn_from);
      while(1) {
        SOCKET inSock = accept(_receiver_socket,(SOCKADDR*)&conn_from, &len);
        _sender_socket = inSock;
        _handler->message(std::string("connected~") + inet_ntoa(conn_from.sin_addr) + ":" + std::to_string(htons(conn_from.sin_port)));
        std::thread t1{process_instream,inSock,_handler};
        t1.detach();
      }
    }

    void Connector::connect_to_server(const std::string& ipaddr, int port, Listener* handler)
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
      std::thread t1{process_instream,_sender_socket,handler};
      t1.detach();

    }


    void Connector::send_message(const std::string& msg)
    {
      send(_sender_socket, msg.c_str(), msg.length()+1,0);
    }


    void Connector::close_down()
    {
      closesocket(_receiver_socket);
      closesocket(_sender_socket);
      WSACleanup();
    }
  }
}

