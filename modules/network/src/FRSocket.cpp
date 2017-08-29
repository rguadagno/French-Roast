// copyright (c) 2017 Richard Guadagno
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

#include <iostream>
#include <string>
#include "FRSocket.h"


namespace frenchroast { namespace network {


    FRSocket::FRSocket(bool create)
    {
      if(create) {
#ifdef CONNECTOR_UNIX
      handle_error((_socket = socket(AF_INET, SOCK_STREAM, 0)), "unable to create socket: ");
      
#else
    _socket = INVALID_SOCKET;
    _socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(_socket == INVALID_SOCKET) {
      std::cout << "socket error : unable to create socket" << std::endl;
      exit(0);
    }

#endif
        _valid = true;

    }
    }


  FRSocket::FRSocket(const FRSocket& ref)
  {
    _valid  = _valid;
    _socket = ref._socket;
    _ipport = ref._ipport;
  }
  
#ifdef CONNECTOR_UNIX
  FRSocket::FRSocket(int sock) : _socket(sock), _valid(true)
  {
  }
#else
  FRSocket::FRSocket(SOCKET sock) : _socket(sock), _valid(true)
  {
  }
  
#endif

  bool FRSocket::is_valid() const 
  {
    return _valid;
  }
  
  void FRSocket::listen()
  {
    handle_error(::listen(_socket, 10),"socket error: cannot listen");
  }


  FRSocket FRSocket::accept()
  {
    struct sockaddr_in    conn_from;
    socklen_t len = sizeof(conn_from);
    FRSocket rv{::accept(_socket, (struct sockaddr*)&conn_from, &len)};
    char buf[INET_ADDRSTRLEN];
    rv._ipport = std::string{inet_ntop(AF_INET, &conn_from.sin_addr, buf, sizeof(buf))} + ":" + std::to_string(htons(conn_from.sin_port));
    return rv;
  }

  const std::string& FRSocket::ipport() const
  {
    return _ipport;
  }
  
  void FRSocket::bind(const std::string& ipaddr, int port)
  {
#ifdef CONNECTOR_UNIX
      struct sockaddr_in serverinfo;
      memset(&serverinfo, 0, sizeof(serverinfo));
      serverinfo.sin_family = AF_INET;
      serverinfo.sin_addr.s_addr = inet_addr(ipaddr.c_str());
      serverinfo.sin_port = htons(port);
      handle_error(::bind(_socket, (struct sockaddr *)&serverinfo, sizeof(serverinfo)), "socket error, unable to bind: ");
#else
      struct sockaddr_in serverinfo;
      memset(&serverinfo, 0, sizeof(serverinfo));
      serverinfo.sin_family = AF_INET;
      inet_pton(AF_INET, ipaddr.c_str(), &serverinfo.sin_addr.s_addr);
      serverinfo.sin_port = htons(port);
      handle_error(::bind(_socket, (struct sockaddr *)&serverinfo, sizeof(serverinfo)), "socket error, unable to bind: ");
#endif 

  }

    void FRSocket::connect(const std::string& ipaddr, int port)
    {
#ifdef CONNECTOR_UNIX
      struct sockaddr_in serverinfo;
      memset(&serverinfo, 0, sizeof(serverinfo));
      serverinfo.sin_family = AF_INET;
      serverinfo.sin_addr.s_addr = inet_addr(ipaddr.c_str());
      serverinfo.sin_port = htons(port);
      handle_error(::connect(_socket, (struct sockaddr *)&serverinfo, sizeof(serverinfo)), "unable to connect to server: ");
#else
      struct sockaddr_in serverinfo;
      memset(&serverinfo, 0, sizeof(serverinfo));
      serverinfo.sin_family = AF_INET;
      inet_pton(AF_INET, ipaddr.c_str(), &serverinfo.sin_addr.s_addr);
      serverinfo.sin_port = htons(port);
      handle_error(::connect(_socket, (struct sockaddr *)&serverinfo, sizeof(serverinfo)), "unable to connect to server: ");
#endif 
    }
    
    bool FRSocket::recv(char* outbuf, int len)
    {
      if(!_valid) return false;
      if(handle_error((_byteCount = ::recv(_socket, outbuf, len, 0)), "socket recv error: ",false)) {
        _valid = false;
        return false;
      }

#ifdef CONNECTOR_UNIX
      if(_byteCount == 0) {
        handle_error(::close(_socket), "socket close error");
        _valid = false;
        return false;
      }

#else
      if(_byteCount == 0) {
        handle_error(closesocket(_socket), "socket close error");
        _valid = false;
        return false;
      }
#endif 
      return true;
    }

    void FRSocket::send(const std::string& str)
    {
      if(!_valid) { std::cout << "cannot send: " << str << std::endl;
return;
}
      handle_error(::send(_socket, str.c_str(), static_cast<int>(str.length()+1),0), "send");
    }

    int FRSocket::bytes_received()
    {
      return _byteCount;
    }


    bool FRSocket::handle_error(int errorid, const std::string& msg, bool exit_on_error)
    {
#ifdef CONNECTOR_UNIX
      if(errorid < 0) {
        std::cout << "socket error: " << msg << "  " << strerror(errno) << std::endl;
        if(exit_on_error) exit(0);
        return true;
      }
#else
      if(errorid < 0 ) { 
        std::cout << "socket error: " << msg << "  " << WSAGetLastError() << std::endl;
        if(exit_on_error) exit(0);
        return true;
      }
#endif
      return false;
    }



  void FRSocket::startup_env()
  {
#ifdef CONNECTOR_UNIX
    ;
#else
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0 ) {
      std::cout << "winsock error: WSAStartup FAILED " << std::endl;
      exit(0);
    }
#endif
  }
  

  void FRSocket::close()
  {
            std::cout << "on demand closed " << std::endl;
    if(!_valid) return;
#ifdef CONNECTOR_UNIX
    handle_error(::close(_socket), "socket close error ",false);
#else
    handle_error(closesocket(_socket), "socket close error ");

#endif
    _valid = false;
  }
    
  }
}
