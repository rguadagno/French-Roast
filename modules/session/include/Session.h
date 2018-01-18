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
#include <unordered_map>
#include "ClassDetail.h"
#include "JammedReport.h"
#include "SignalReport.h"
#include "MethodStats.h"
#include "Persistor.h"

namespace frenchroast { namespace session {
    class Session  {

      std::vector<frenchroast::monitor::ClassDetail>                      _loaded_classes;
      std::unordered_map<std::string, frenchroast::monitor::JammedReport> _jammed;
      std::vector<frenchroast::monitor::StackTrace>                       _traffic;
      std::vector<frenchroast::monitor::MethodStats>                      _method_rankings;
      std::unordered_map<std::string, frenchroast::monitor::SignalReport> _signals;
      std::string              _descriptor{""};
      std::size_t              _seconds{0};
      Persistor*               _persistor{nullptr};
  
    public:
      Session() = default;
      Session(Persistor*);
      Session(Persistor*, const std::string& descriptor, std::size_t seconds);
      Session(const Session&);
      ~Session();
      void update(const std::vector<frenchroast::monitor::ClassDetail>& details);
      void update(const frenchroast::monitor::JammedReport& );
      void update(const std::vector<frenchroast::monitor::StackTrace>& traffic);
      void update(const std::vector<frenchroast::monitor::MethodStats>& methods);
      void update(const frenchroast::monitor::SignalReport&);
      std::vector<frenchroast::monitor::ClassDetail>                      get_loaded_classes() const;
      std::unordered_map<std::string, frenchroast::monitor::JammedReport> get_jammed_reports() const;
      std::vector<frenchroast::monitor::StackTrace>                       get_traffic() const;
      std::vector<frenchroast::monitor::MethodStats>                      get_rankings() const;
      std::unordered_map<std::string, frenchroast::monitor::SignalReport> get_signal_reports() const;
      void store(const std::string&);
      void store();
      void load(const std::string&);
      void reset();
      bool operator==(const Session& ref) const;
      bool operator!=(const Session& ref) const;
      bool has_descriptor() const;
    };
    
  }
}
#endif
