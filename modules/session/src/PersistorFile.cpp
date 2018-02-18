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


#include <iostream>
#include <fstream>
#include "PersistorFile.h"
#include "Util.h"

namespace frenchroast { namespace session {
    using namespace frenchroast::monitor;
    
    void PersistorFile::store(const std::string& fileName, std::vector<std::string> items)
    {
      std::ofstream out{fileName};
      if(!out) throw std::invalid_argument{"cannot open " + fileName + " for update"};
      for(auto& item : items) {
        out << item << std::endl;
      }
    }
    
    void PersistorFile::load(const std::string& fileName)
    {
      get_all_views(fileName, _views);
    }
    
    void PersistorFile::store()
    {
    }

    const std::string& PersistorFile::get_serial(char tag)
    {
      return _views[tag];
    }

    void PersistorFile::get_all_views(const std::string& fileName, std::unordered_map<char, std::string>& views)
    {
      std::ifstream in{fileName};
      if(!in) throw std::invalid_argument{"cannot open " + fileName + " for reading"};
      std::string line;
      while(getline(in,line)) {
        if(line.size() < 2) continue;
        views[line[0]] = line.substr(1);
      }
    }
    
  }
}
