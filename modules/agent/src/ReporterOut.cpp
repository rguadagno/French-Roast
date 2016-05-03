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
#include "ReporterImp.h"
#include <iostream>

namespace frenchroast { namespace agent {

    //    void ReporterOut::init(const std::string& initinfo)
    // {
    //}  
    void ReporterOut::traffic(const std::string& tag) {}
    void ReporterOut::signal(const std::string& tag)
    {
      std::cout << tag << std::endl;
    }
    
    void ReporterOut::signal_timer(long long xtime, const std::string& direction, const std::string& tag, const std::string threadname)
    {
      std::cout << direction << "  " << tag << "  " << xtime << std::endl;
    }

    void ReporterOut::close()
    {
    }
  }
}
