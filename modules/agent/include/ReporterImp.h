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

#ifndef RRPTIMP_H
#define RRPTIMP_H
#include <string>
#include <iostream>
#include <fstream>

#include "Reporter.h"
#include "Connector.h"

namespace frenchroast { namespace agent {

    class ReporterOut : public Reporter {
    public:
      void init(const std::string& initinfo);
      void signal(const std::string& tag);
      void signal_timer(long long time, const std::string& direction, const std::string& tag, const std::string threadname);
      void close();
    };

    class ReporterFile : public Reporter {
      std::ofstream _outfile;
    public:
      void init(const std::string& initinfo);
      void signal(const std::string& tag);
      void signal_timer(long long time, const std::string& direction, const std::string& tag, const std::string threadname);
      void close();
    };

    class ReporterServer : public Reporter {
      network::Connector _conn;
    public:
      void init(const std::string& initinfo);
      void signal(const std::string& tag);
      void signal_timer(long long time, const std::string& direction, const std::string& tag, const std::string threadname);
      void close();
    };

  }
}

#endif
