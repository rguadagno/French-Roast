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

#ifndef RRPT_H
#define RRPT_H
#include <string>
#include <vector>
#include "Transport.h"
#include "ClassDetail.h"

namespace frenchroast { namespace agent {

        
    class Reporter {
      Transport* _ptr;
    public:
      void traffic(const std::string& tag);
      void loaded_classes(std::vector<frenchroast::monitor::ClassDetail> details);
      void ready();
      void setTransport(Transport* ptr);
      void signal(const std::string& tag);
      void signal_timer(long long time, const std::string& direction, const std::string& tag, const std::string threadname);
      void close();
      void unloaded(const std::string& msg);
      void jammed(const std::string& monitor, const std::string& waiter, const std::string& owner);

    };

  }
}

#endif
