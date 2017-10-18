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

#ifndef MOCKSOCK_H
#define MOCKSOCK_H

#ifdef CONNECTOR_UNIX
  #include <sys/socket.h>
  #include <sys/types.h>
  #include <unistd.h>
  #include <netinet/in.h>
  #include <arpa/inet.h>
  #include <string.h>
#else
  #include <winsock2.h>
  #include <ws2tcpip.h>
#endif 


namespace frenchroast { namespace network {

    class MockSocket {

    public:
      
      static int _maxAccept;
      static int _maxRecv;
      static int _recvLen[5];
      static std::string _recvOut[5];
      int _acceptCount{0};
      int _recvCount{0};
      
      bool _valid{false};
      int _byteCount;
      std::string _ipport;
      bool handle_error(int errorid, const std::string& msg, bool exit_on_error = true);
      

      MockSocket(bool create = false);
      MockSocket(const MockSocket&);
      bool is_valid() const;
      static void startup_env();
#ifdef CONNECTOR_UNIX      
      MockSocket(int);
#else
      MockSocket(SOCKET);      
#endif       
      bool recv(char* outbuf, int len);
      void send(const std::string&);
      int bytes_received();
      void connect(const std::string& ipaddr, int port);
      void bind(const std::string& ipaddr, int port);
      MockSocket accept();
      void listen();
      void close();
      const std::string& ipport() const;

    };


    
  }
}

#endif
