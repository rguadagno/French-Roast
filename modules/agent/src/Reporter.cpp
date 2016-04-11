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
#include "ReporterImp.h"

#include "Util.h"

namespace frenchroast { namespace agent {

    void Reporter::init(const std::string& initinfo)
    {
      std::vector<std::string> items = frenchroast::split(initinfo,"~");
      if (items[0] == "out") {
        _impPtr = new ReporterOut();
      }
      if (items[0] == "file") {
	_impPtr = new ReporterFile();
	_impPtr->init(items[1]);
      }
      if (items[0] == "server") {
	_impPtr = new ReporterServer();
	_impPtr->init(items[1]);
      }
    }  

    void Reporter::signal(const std::string& tag)
    {
      _impPtr->signal(tag);
    }
    
    void Reporter::signal_timer(long long time, const std::string& direction, const std::string& tag, const std::string threadname)
    {
      _impPtr->signal_timer(time, direction, tag, threadname);
    }

    void Reporter::close()
    {
      _impPtr->close();
    }

  }
}
