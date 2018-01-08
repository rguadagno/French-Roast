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
    
    const  std::string PersistorFile::TAG_LOADED_CLASSES = "<loaded-classes>";
    const  std::string PersistorFile::TAG_JAMMED         = "<jammed>";
    const  std::string PersistorFile::TAG_VIEW           = "<view>";
    
    void PersistorFile::store(const std::string& fileName,
                              const std::vector<ClassDetail>& loaded_classes,
                              const std::unordered_map<std::string, JammedReport>& jammed)
    {
      std::ofstream out{fileName};
      if(!out) throw std::invalid_argument{"cannot open " + fileName + " for update"};
      
      out << TAG_LOADED_CLASSES << TAG_VIEW;
      for(auto& x : loaded_classes) {
        out << x << ClassDetail::TAG_END;
      }
      out << std::endl << TAG_JAMMED << TAG_VIEW;
      for(auto& x : jammed) {
        out << x.second << JammedReport::TAG_END;
      }
    }
    
    void PersistorFile::load(const std::string& fileName,
                             std::vector<ClassDetail>& loaded_classes,
                             std::unordered_map<std::string, JammedReport>& jammed)
    {
      std::unordered_map<std::string, std::string> views;
      get_all_views(fileName, views);
      load_loaded_classes(views[TAG_LOADED_CLASSES], loaded_classes);
      load_jammed(views[TAG_JAMMED], jammed);
    }


    void PersistorFile::store()
    {
    }


    void PersistorFile::load_jammed(const std::string& line, std::unordered_map<std::string,JammedReport>& jammed)
    {
      for(auto& jam : frenchroast::split(line, JammedReport::TAG_END)) {
        if(jam != "") {
          frenchroast::monitor::JammedReport jr;
          jam >> jr;
          jammed[jr.key()] = jr;
        }
      
      }
      
    }
    
    void PersistorFile::load_loaded_classes(const std::string& line , std::vector<ClassDetail>& loaded_classes)
    {
      for(auto& detail : frenchroast::split(line, ClassDetail::TAG_END)) {
        if(detail != "") {
          ClassDetail cd;
          detail >> cd;
          loaded_classes.push_back(cd);
        }
      
      }
    }

    void PersistorFile::get_all_views(const std::string& fileName, std::unordered_map<std::string, std::string>& views)
    {
      std::ifstream in{fileName};
      if(!in) throw std::invalid_argument{"cannot open " + fileName + " for reading"};
      std::string line;
      while(getline(in,line)) {
        views[frenchroast::split(line, TAG_VIEW)[0]] = frenchroast::split(line, TAG_VIEW)[1];
      }
    }
    
  }
}
