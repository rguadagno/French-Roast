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
//    along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
//

#include <thread>
#include <chrono>
#include <vector>
#include <mutex>
#include "catch.hpp"
#include "Connector.h"
#include "MockSocket.h"

using namespace frenchroast::network;


class MockListener : public Listener
{
  std::mutex _lock;
  std::vector<std::string> _msgs;
public:
  std::vector<std::string> messages()
  {
    std::lock_guard<std::mutex> lck{_lock};
    return _msgs;
  }
  
  void message(const std::string& msg) {
    _msgs.push_back(msg);
  }
 };



TEST_CASE("wait for client connection")
{
  Connector<MockSocket> conn;
  MockSocket::_maxAccept = 1;
  MockSocket::_maxRecv = 0;
  MockListener ml{};
  using ConnMember = void (Connector<MockSocket>::*)(const std::string& , int , Listener*,bool);
  ConnMember ptr = &Connector<MockSocket>::wait_for_client_connection;
  std::thread t1{[&](){ (&conn->*ptr)("127.0.0.1", 6060, &ml,true);} } ;
  
  t1.detach();
  std::this_thread::sleep_for(std::chrono::milliseconds(20));
  std::vector<std::string> msgs = ml.messages();
  REQUIRE(msgs.size() == 1);
  REQUIRE(msgs[0].find("connected~") != std::string::npos);
}



TEST_CASE("recv one shot: some text")
{
  Connector<MockSocket> conn;
  MockSocket::_maxAccept = 1;
  MockSocket::_maxRecv = 1;
  std::string outStr = "some text";
  MockSocket::_recvOut[0]  = outStr;
  MockSocket::_recvLen[0]  = outStr.size()+1;
  MockListener ml{};
  using ConnMember = void (Connector<MockSocket>::*)(const std::string& , int , Listener*,bool);
  ConnMember ptr = &Connector<MockSocket>::wait_for_client_connection;
  std::thread t1{[&](){ (&conn->*ptr)("127.0.0.1", 6060, &ml,true);} } ;
  
  t1.detach();
  std::this_thread::sleep_for(std::chrono::milliseconds(20));
  std::vector<std::string> msgs = ml.messages();
  REQUIRE(msgs.size() == 2);
  REQUIRE(msgs[1].find("some text") != std::string::npos);
}

TEST_CASE("recv two shots: some text")
{
  Connector<MockSocket> conn;
  MockSocket::_maxAccept = 1;
  MockSocket::_maxRecv = 2;
  MockSocket::_recvOut[0]  = "some";
  MockSocket::_recvLen[0]  = 4;
  MockSocket::_recvOut[1]  = " text";
  MockSocket::_recvLen[1]  = 6;

  MockListener ml{};
  using ConnMember = void (Connector<MockSocket>::*)(const std::string& , int , Listener*,bool);
  ConnMember ptr = &Connector<MockSocket>::wait_for_client_connection;
  std::thread t1{[&](){ (&conn->*ptr)("127.0.0.1", 6060, &ml,true);} } ;
  
  t1.detach();
  std::this_thread::sleep_for(std::chrono::milliseconds(20));
  std::vector<std::string> msgs = ml.messages();
  REQUIRE(msgs.size() == 2);
  REQUIRE(msgs[1].find("some text") != std::string::npos);
}

TEST_CASE("recv two shots: not all text")
{
  Connector<MockSocket> conn;
  MockSocket::_maxAccept = 1;
  MockSocket::_maxRecv = 2;
  MockSocket::_recvOut[0]  = "not all";
  MockSocket::_recvLen[0]  = 8;
  MockSocket::_recvOut[1]  = " text";
  MockSocket::_recvLen[1]  = 5;

  MockListener ml{};
  using ConnMember = void (Connector<MockSocket>::*)(const std::string& , int , Listener*,bool);
  ConnMember ptr = &Connector<MockSocket>::wait_for_client_connection;
  std::thread t1{[&](){ (&conn->*ptr)("127.0.0.1", 6060, &ml,true);} } ;
  
  t1.detach();
  std::this_thread::sleep_for(std::chrono::milliseconds(20));
  std::vector<std::string> msgs = ml.messages();
  REQUIRE(msgs.size() == 2);
  REQUIRE(msgs[1].find("not all") != std::string::npos);
}

