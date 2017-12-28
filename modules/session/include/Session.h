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

#ifndef SESSION_H
#define SESSION_H

#include <vector>
#include "ClassDetail.h"
#include "Persistor.h"

namespace frenchroast { namespace session {
    class Session  {

      std::vector<frenchroast::monitor::ClassDetail> _loaded_classes;
      std::string              _descriptor{""};
      std::size_t              _seconds{0};
      Persistor*               _persistor{nullptr};
  
    public:
      Session() = default;
      Session(Persistor*);
      Session(Persistor*, const std::string& descriptor, std::size_t seconds);
      Session(const Session&);
      ~Session();
      void update_class_viewer(const std::vector<frenchroast::monitor::ClassDetail>& details);
      std::vector<frenchroast::monitor::ClassDetail> get_class_viewer() const;
      void store(const std::string&);
      void load(const std::string&);     
      bool operator==(const Session& ref) const;
      bool operator!=(const Session& ref) const;
    };
    
  }
}
#endif
