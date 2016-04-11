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
#include <vector>
#include "ReporterImp.h"
#include "Util.h"

namespace frenchroast { namespace agent {

    void ReporterServer::init(const std::string& serverinfo)
    {
      std::vector<std::string> items = frenchroast::split(serverinfo,":");
      if (items.size() != 3) {
	 throw std::invalid_argument("ReporterSever, bad server info: " + serverinfo);
      }
      if (items[0] != "send") {
	 throw std::invalid_argument("ReporterSever, bad server info: " + serverinfo);
      }
      _conn.init_sender(items[1], atoi(items[2].c_str()));
    }  

    void ReporterServer::signal(const std::string& tag)
    {
      _conn.send_message("signal~" + tag);
    }
    
    void ReporterServer::signal_timer(long long xtime, const std::string& direction, const std::string& tag, const std::string threadname)
    {
      _conn.send_message("signaltimer~" + frenchroast::ntoa(xtime) +"~" + direction + "~" + tag + "~" + threadname);
    }
    
    void ReporterServer::close()
    {
      _conn.close_down();
    }

  }
}
