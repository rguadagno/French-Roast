// copyright (c) 2018 Richard Guadagno
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
#include "InstrumentationReport.h"
#include "Util.h"

namespace frenchroast { namespace monitor {
    const std::string InstrumentationReport::DESCRIPTOR_DELIM = "<end-descriptor>";
    InstrumentationReport::InstrumentationReport(const std::string& hostname, const std::string& pid,const std::string& class_name, const std::vector<signal::Signal>& sigs) : _hostname(hostname), _pid(pid)
    {
      for(auto& x :sigs) {
        if(x.valid()) {
          _valid.push_back(Descriptor{class_name + "::" + x.method_name()});
        }
        else {
          _invalid.push_back(Descriptor{class_name + "::" + x.method_name()});
        }
      }
    }
    
    const std::vector<Descriptor>& InstrumentationReport::valid() const
    {
      return _valid;
    }
    
    const std::vector<Descriptor>& InstrumentationReport::invalid() const
    {
      return _invalid;
    }
    
    const std::string& InstrumentationReport::hostname() const
    {
      return _hostname;
    }
    
    const std::string& InstrumentationReport::pid() const
    {
      return _pid;
    }

    InstrumentationReport& operator>>(const std::string& rep, InstrumentationReport& ref)
    {
      auto parts = split(rep.substr(1), "<end-pid>");
      ref._hostname = split(parts[0],"<end-hostname>")[0];
      ref._pid = split(parts[0],"<end-hostname>")[1];
      parts = split(split(rep.substr(1), "<end-pid>")[1],"<end-valid>");
      for(auto& x : split(parts[0], InstrumentationReport::DESCRIPTOR_DELIM)) {
        if(x == "") continue;
        ref._valid.push_back(x >> Descriptor{});
      }
      for(auto& x : split(parts[1], InstrumentationReport::DESCRIPTOR_DELIM)) {
        if(x == "") continue;
        ref._invalid.push_back(x >> Descriptor{});
      }
      return ref;
    }
    
    InstrumentationReport& operator>>(const std::string& rep, InstrumentationReport&& ref)
    {
      return rep >> ref;
    }

    

  }
}
