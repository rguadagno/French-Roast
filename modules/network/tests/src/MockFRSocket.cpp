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
#include <thread>
#include <chrono>
#include "MockSocket.h"
#include "FRSocket.h"


namespace frenchroast { namespace network {

    int MockSocket::_maxAccept;
    int MockSocket::_maxRecv;
    int MockSocket::_recvLen[5];
    std::string MockSocket::_recvOut[5];
    

    MockSocket::MockSocket(bool create)
    {
      _valid = true;
    }


    MockSocket::MockSocket(const MockSocket& ref)
    {
      _valid  = _valid;
      _ipport = ref._ipport;
    }
  
#ifdef CONNECTOR_UNIX
    MockSocket::MockSocket(int sock) : _valid(true)
    {
    }
#else
    MockSocket::MockSocket(SOCKET sock) : _valid(true)
    {
    }
  
#endif

    bool MockSocket::is_valid() const 
    {
      return _valid;
    }
    
    void MockSocket::listen()
    {
    }

    
    MockSocket MockSocket::accept()
    {
      if(_acceptCount >= _maxAccept) std::this_thread::sleep_for(std::chrono::seconds(5000));
      
      MockSocket rv{true};
      ++_acceptCount;
      return rv;
    }

    const std::string& MockSocket::ipport() const
    {
      return _ipport;
    }
  
    void MockSocket::bind(const std::string& ipaddr, int port)
    {
    }

    void MockSocket::connect(const std::string& ipaddr, int port)
    {
    }
    
    bool MockSocket::recv(char* outbuf, int len)
    {
       if(++_recvCount > _maxRecv)
        return false;

       memcpy(outbuf, _recvOut[_recvCount-1].c_str(), _recvLen[_recvCount-1]);
      _byteCount = _recvLen[_recvCount-1];
      return true;
    }

    void MockSocket::send(const std::string& str)
    {
    }

    int MockSocket::bytes_received()
    {
      return _byteCount;
    }


    bool MockSocket::handle_error(int errorid, const std::string& msg, bool exit_on_error)
    {
      return false;
    }



    void MockSocket::startup_env()
    {
    }
  

    void MockSocket::close()
    {
    }
    
  }
}
