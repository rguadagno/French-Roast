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
#include "ClassDetail.h"
#include "Connector.h"

namespace frenchroast { namespace agent {

    void Reporter::setTransport(Transport* ptr)
    {
      if(ptr == nullptr) throw std::invalid_argument("Transport is nullptr");
      _ptr = ptr;
    }


    void Reporter::loaded_classes(std::vector<frenchroast::monitor::ClassDetail> details)
    {
      if(details.size() == 0) return;

      std::string outstr = "";
      for(auto& citem : details) {
        outstr.append(citem.name() + "^[");
        for(auto& meth : citem.methods()) {
          outstr.append(meth + "%");
        }
        outstr.append("]^");
      }
      
      _ptr->out("loaded~" + outstr);
    }
    
    void Reporter::signal(const std::string& tag)
    {
      _ptr->out("signal~" + tag);
    }

    void Reporter::traffic(const std::string& tag)
    {
      _ptr->out("traffic~" + tag);
    }
    
    void Reporter::jammed(const std::string& monitor, const std::string& waiter, const std::string& owner)
    {
      _ptr->out("jammed~" + monitor + "~" + waiter + "~" + owner);
    }
    
    void Reporter::signal_timer(long long xtime, const std::string& direction, const std::string& tag, const std::string threadname)
    {
      _ptr->out("signaltimer~" + std::to_string(xtime) +"~" + direction + "~" + tag + "~" + threadname);
    }

    void Reporter::unloaded(const std::string& msg)
    {
      using  namespace frenchroast::network;
      _ptr->out("unloaded~" + Connector::get_hostname() + "~" +       std::to_string(Connector::get_pid()));
    }

    void Reporter::ready()
    {
      using  frenchroast::network::Connector;
      _ptr->out("ready~" + Connector::get_hostname() + "~" +       std::to_string(Connector::get_pid()));
    }

    
    //    void Reporter::close()
    //{
    //_impPtr->close();
    //}

  }
}


