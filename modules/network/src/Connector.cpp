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


#ifdef CONNECTOR_UNIX
  #include <sys/socket.h>
  #include <sys/types.h>
  #include <unistd.h>
#else
  #include <sys/types.h>
  #include <winsock2.h>
  #include <process.h>
#endif 

#include <iostream>
#include <functional>
#include <cstring>
#include <iostream>
#include <thread>
#include "Listener.h"
#include "FRSocket.h"
#include "Connector.h"

namespace frenchroast { namespace network {

    void Connector::wait_for_client_connection(const std::string& ipaddr, int port, Listener* handler, bool silent)
    {
      FRSocket::startup_env();
      _handler = handler;
      _receiver_socket = FRSocket{true};
      _receiver_socket.bind(ipaddr, port);
      _receiver_socket.listen();
      while(1) {
        FRSocket inSock = _receiver_socket.accept();
        _ipport_sendersocket[inSock.ipport()] = inSock;
        _handler->message("connected~" + inSock.ipport());
        std::thread t1{process_instream, std::ref(inSock), inSock.ipport(), _handler,std::ref(_ipport_sendersocket)};
        t1.detach();
      }
    }

    void Connector::connect_to_server(const std::string& ipaddr, int port, Listener* handler)
    {
      FRSocket::startup_env();
      _ipport_sendersocket["client"] = FRSocket{true};
      _ipport_sendersocket["client"].connect(ipaddr,port);
      std::cout << "========== CONNECTED TO SERVER ============ " << std::endl;
      if(handler != nullptr) {
        std::thread t1{process_instream,std::ref(_ipport_sendersocket["client"]),"",handler,std::ref(_ipport_sendersocket)};
        t1.detach();
      }
    }

    
    void Connector::blocked_listen(Listener* handler)
    {
      process_instream(std::ref(_ipport_sendersocket["client"]),"", handler,std::ref(_ipport_sendersocket));
    }

    void Connector::send_message(const std::string& msg)
    {
      for(auto& s : _ipport_sendersocket) {
        if(s.second.is_valid()) {
          s.second.send(msg);
        }
      }
    }

    void Connector::send_message(const std::string& ip_port, const std::string& msg)
    {
      _ipport_sendersocket[ip_port].send(msg);
    }

    void Connector::close_down()
    {
      _ipport_sendersocket["client"].close();
      WSACleanup();
    }

    int Connector::get_pid()
    {
#ifdef CONNECTOR_UNIX
      return getpid();
#else
      return _getpid();
#endif    
    }


    std::string Connector::get_hostname()
    {
      char buf[100];
      gethostname(buf, 100);
      return std::string{buf};
    }

    
    void process_instream(FRSocket& insock, const std::string ipport, Listener* handler,std::unordered_map<std::string, FRSocket>& sockets) 
    {
      char databuf[3000];
      char flowbuf[6000];
      char strbuf[2000];
      int start = 0;
      int end = 0;
      int buflen = 0;
      
      memset(databuf,0,sizeof(databuf));
      int total = 0;
      while(insock.recv(databuf, sizeof(databuf))) {
        int rv = insock.bytes_received();
        total += rv;
        memcpy(&flowbuf[buflen], databuf, rv);
        int total_bytes = buflen + rv;
        while(end < total_bytes) {
          if (flowbuf[end] == '\0') {
            memcpy(strbuf, &flowbuf[start],(end-start) + 1);
            std::string item{strbuf};
            handler->message(ipport+"~"+item);
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

  }
}
