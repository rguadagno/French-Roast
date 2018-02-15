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
#include "Reporter.h"
#include "Connector.h"
#include "Command.h"
#include "Util.h"

namespace frenchroast { namespace agent {


    Reporter::Reporter(network::Connector<>& conn) : _conn(conn)
    {
    }
    
    void Reporter::signal(const std::string& tag)
    {
      if(tag.size() > frenchroast::network::MAX_MSG_SIZE) {
        for(auto& item : split(tag, "<end>")) {
          _conn.send_message(item);
        }
      }
      else {
        _conn.send_message(tag);
      }
    }

    void Reporter::unloaded(const std::string& msg)
    {
      using  namespace frenchroast::network;
      _conn.send_message(frenchroast::monitor::command::UNLOADED + "~" + Connector<>::get_hostname() + "~" +       std::to_string(Connector<>::get_pid()));
    }

    void Reporter::ready()
    {
      using  frenchroast::network::Connector;
      using  namespace frenchroast::network;
      _conn.send_message(frenchroast::monitor::command::READY + "~" + Connector<>::get_hostname() + "~" +       std::to_string(Connector<>::get_pid()));
    }
  }
}


