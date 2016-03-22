#include <winsock2.h>
#include <windows.h>
#include <iostream>
#include <ws2tcpip.h>
#include <sstream>
#include "Reporter.h"
#include "Util.h"



namespace frenchroast { namespace monitoring {


    SOCKET _sender_socket;
    
    void Reporter::init(const std::string& initInfo)
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
      connectInfo.sin_addr.s_addr = inet_addr(frenchroast::split(initInfo,':')[0].c_str());  // @@ fix this
      std::cout << "========== PORT = " << split(initInfo,':')[0] << std::endl;
      connectInfo.sin_port        = htons(atoi(split(initInfo,':')[1].c_str()));
      int rv = connect(_sender_socket, (SOCKADDR*)&connectInfo, sizeof(connectInfo));
      if (rv != 0) {
	std::cout << "========== Unable to connect to server ============ " << std::endl;
	exit(0);
      }
      std::cout << "========== CONNECTED TO SERVER ============ " << std::endl;
    }
    
    void Reporter::hook(const std::string& descriptor)
    {
      send(_sender_socket, descriptor.c_str(), descriptor.length()+1,0);
    }


  }
}
