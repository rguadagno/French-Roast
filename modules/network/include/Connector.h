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
#include <thread>
#include <iostream>
#include <unordered_map>
#ifdef CONNECTOR_UNIX
  #include <sys/socket.h>
  #include <sys/types.h>
  #include <unistd.h>
#else
  #include <sys/types.h>
  #include <winsock2.h>
  #include <process.h>
#endif 

#include "Listener.h"
#include "FRSocket.h"

namespace frenchroast { namespace network {
    
    template
      <typename SocketType>
      void process_instream(std::string ipport, Listener* handler,std::unordered_map<std::string, SocketType>& sockets)
      {
        char databuf[3000];
        char flowbuf[6000];
        char strbuf[2000];
        int start = 0;
        int end = 0;
        int buflen = 0;
      
        memset(databuf,0,sizeof(databuf));
        int total = 0;
        std::string str = "";
        while(sockets[ipport].recv(databuf, sizeof(databuf))) {
          int rv = sockets[ipport].bytes_received();
          total += rv;
          memcpy(&flowbuf[buflen], databuf, rv);
          int total_bytes = buflen + rv;
           
          while(end < total_bytes) {
            if (flowbuf[end] == '\0') {
              str.clear();
              str.append(ipport);
              str.append("~");
              memcpy(strbuf, &flowbuf[start],(end-start) + 1);
              str.append(strbuf);
              handler->message(str);
              start = end + 1;
            }
            ++end;
          }
          buflen = (end - start);
          if(buflen > 0) {
            memcpy(flowbuf, &flowbuf[end - buflen], buflen);
          }
          end = 0;
          start = 0;
        }
        sockets.erase(ipport);
      }

    template
      <typename SocketType = FRSocket>
    class Connector {
      SocketType  _receiver_socket;
      std::unordered_map<std::string, SocketType> _ipport_sendersocket;
      Listener* _handler;
   
    public:
      void wait_for_client_connection(const std::string& ipaddr, int port, Listener* handler, bool silent=true)
      {
        SocketType::startup_env();
        _handler = handler;
        _receiver_socket = SocketType{true};
        _receiver_socket.bind(ipaddr, port);
        _receiver_socket.listen();
        while(1) {
          SocketType inSock = _receiver_socket.accept();
          _ipport_sendersocket[inSock.ipport()] = inSock;
          _handler->message("connected~" + inSock.ipport());
          std::thread t1{process_instream<SocketType>, inSock.ipport(), _handler,std::ref(_ipport_sendersocket)};
          t1.detach();
        }
      }
      
      void connect_to_server(const std::string& ipaddr, int port, Listener* handler = nullptr)
      {
        SocketType::startup_env();
        _ipport_sendersocket["client"] = SocketType{true};
        _ipport_sendersocket["client"].connect(ipaddr,port);
        std::cout << "========== CONNECTED TO SERVER ============ " << std::endl;
        if(handler != nullptr) {
          std::thread t1{process_instream<SocketType>,"client", handler, std::ref(_ipport_sendersocket)};
          t1.detach();
        }
      }
      
      void blocked_listen(Listener* handler)
      {
        process_instream<FRSocket>("client", handler,std::ref(_ipport_sendersocket));
      }
      
      void send_message(const std::string& msg)
      {
        for(auto& s : _ipport_sendersocket) {
          if(s.second.is_valid()) {
            s.second.send(msg);
          }
        }
      }
      
      void send_message(const std::string& ip_port, const std::string& msg)
      {
        _ipport_sendersocket[ip_port].send(msg);
      }
      
      void close_down()
      {
        _ipport_sendersocket["client"].close();
#ifdef CONNECTOR_UNIX      
        ;
#else
        WSACleanup();
#endif
      }
      
      static std::string get_hostname()
      {
        char buf[100];
        gethostname(buf, 100);
        return std::string{buf};
      }

      static int get_pid()
      {
#ifdef CONNECTOR_UNIX
        return getpid();
#else
        return _getpid();
#endif    
      }
    };
  }
}

#endif
