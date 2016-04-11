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

#ifndef RPT_H
#define RPT_H
#include <string>

namespace frenchroast { namespace agent {

    class Config {
      std::string _reporterDescriptor;
      std::string _opcodeFile;
      std::string _hooksFile;
    public:
      Config();
      bool load(const std::string& configFile);
      std::string get_reporter_descriptor() const;
      std::string get_opcode_file() const;
      std::string get_hooks_file() const;
    };
  }
}



#endif
