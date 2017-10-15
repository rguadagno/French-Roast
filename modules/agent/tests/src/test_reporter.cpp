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

#include "catch.hpp"
#include "Transport.h"
#include "Reporter.h"


class TransportMock : public frenchroast::agent::Transport {
public:
  std::string _str;
  void out(const std::string& str) { _str = str;}
};

TEST_CASE("setTransport nullptr")
{
  frenchroast::agent::Reporter rptr;
  REQUIRE_THROWS_AS(rptr.setTransport(nullptr),  std::invalid_argument);
}

TEST_CASE("signal")
{
  frenchroast::agent::Reporter rptr;
  TransportMock mock;
  rptr.setTransport(&mock);
  rptr.signal("tester");
  REQUIRE(mock._str == "tester");
  
}


TEST_CASE("unloaded")
{
  frenchroast::agent::Reporter rptr;
  TransportMock mock;

  rptr.setTransport(&mock);
  rptr.unloaded("");
  REQUIRE(mock._str.find("unloaded~") != std::string::npos);

}

TEST_CASE("ready")
{
  frenchroast::agent::Reporter rptr;
  TransportMock mock;

  rptr.setTransport(&mock);
  rptr.ready();
  REQUIRE(mock._str.find("ready~") != std::string::npos);

}
