// copyright (c) 2017 Richard Guadagno
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

#ifndef PERSISTORFILE_H
#define PERSISTORFILE_H


#include "Persistor.h"

namespace frenchroast { namespace session {

    class PersistorFile : public Persistor {
      std::string generate_name();
    public:
      // use generated file name
      PersistorFile(const std::string& dirName);
      // full path and file name will be provied in store(std::string)
      PersistorFile() {}
      // user selected full path to file
      void store(const std::string&, const std::vector<frenchroast::monitor::ClassDetail>&);
      // will use generated name, called periodically when in headless mode
      void store();
      
      void load(const std::string& fileName, std::vector<frenchroast::monitor::ClassDetail>& loaded_classes);
      ~PersistorFile() {}
};

  }
}
#endif
