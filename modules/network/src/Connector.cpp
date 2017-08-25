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

#include <iostream>
#include "Listener.h"
#include <cstring>
#include <iostream>


#ifdef CONNECTOR_UNIX
  #include <sys/socket.h>
  #include <sys/types.h>
  #include <unistd.h>
#else
  #include <winsock2.h>
#endif 

namespace frenchroast { namespace network {
    void process_instream(int connfd, const std::string ipport, Listener* handler) 
    {
      int rv;
      char databuf[3000];
      char flowbuf[6000];
      char strbuf[2000];
      int start = 0;
      int end = 0;
      int buflen = 0;
      
      memset(databuf,0,sizeof(databuf));
      int total = 0;
      while(1) {
        rv = recv(connfd, databuf, sizeof(databuf), 0);
        total += rv;
        if (rv < 0 ) {
          std::cout << "RECV ERROR: " << std::endl;
          break;
        }
        if (rv == 0 ) {
          #ifdef CONNECTOR_UNIX
            close(connfd);
          #else
            closesocket(connfd);
          #endif
          std::cout << "CLOSED. " << std::endl;
          break;
        }

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
  }

  }
}
