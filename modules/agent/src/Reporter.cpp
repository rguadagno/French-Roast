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
#include "Util.h"

namespace frenchroast { namespace agent {
    /*
    void Reporter::init(const std::string& initinfo,CommandListener* cl)
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
        _impPtr->init(items[1],cl);
      }
    }  
    */
    void Reporter::setTransport(Transport* ptr)
    {
      if(ptr == nullptr) throw std::invalid_argument("Transport is nullptr");
      _ptr = ptr;
    }
    
    void Reporter::signal(const std::string& tag)
    {
      _ptr->out("signal~" + tag);
    }

    void Reporter::traffic(const std::string& tag)
    {
      _ptr->out("traffic~" + tag);
    }
    
    void Reporter::signal_timer(long long xtime, const std::string& direction, const std::string& tag, const std::string threadname)
    {
      _ptr->out("signaltimer~" + frenchroast::ntoa(xtime) +"~" + direction + "~" + tag + "~" + threadname);
    }

    //    void Reporter::close()
    //{
    //_impPtr->close();
    //}

  }
}


