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

#ifndef FRSIGS_H
#define FRSIGS_H

#include <unordered_map>
#include <vector>
#include <unordered_map>
#include <regex>
#include <bitset>


namespace frenchroast { namespace signal {
    class Signal {
      int            _line;
      std::string    _name;
      bool           _includeArtifacts;
      std::bitset<4> _flags;
      bool           _all{false};
    public:
      Signal(const std::string& name, int lineno);
      Signal(const std::string& name, std::bitset<4>, bool artifacts);
      int            line_number() const;
      std::string    method_name() const;
      std::bitset<4> flags() const;
      bool all() const;
      bool artifacts() const;
    };
    
    class Signals {
      std::regex  _fullRegex{"((?:[A-Za-z0-9_]+\\.)+[A-Za-z0-9]+)::([A-Za-z0-9_\\*]+(?::\\([a-z]*\\):[a-z]+){0,})(<ENTER>|<EXIT>|<TIMER>){1}((?:\\[[A-Za-z0-9_]+\\]){0,})((?:<ARTIFACTS:OFF>){0,})"};
      std::regex  _methodRegex{"((?:[A-Za-z0-9_]+\\.)+[A-Za-z0-9]+)::([A-Za-z0-9_]+:\\([a-z]*\\):[a-z]+)(.*)"};
      static std::unordered_map<std::string, std::string>      _type_map;
      std::unordered_map<std::string,std::vector<Signal>>      _hlist;
      std::unordered_map<std::string,std::vector<std::string>> _markerFields;
      
      void validate(const std::string& line, std::string& classstr,std::string& methodstr,std::string& flagstr,std::string& fieldstr, std::string& artifactStr);
      std::string convert_name(const std::string& name);
      std::vector<std::string> parse_token_types(const std::string& pstr);
      void parse_flags(std::bitset<4>& flags, std::string str);
      
    public:
      bool is_signal_class(const std::string& name) const;
      std::vector<std::string> classes() const;
      const std::vector<Signal>& operator[](const std::string& name);
      void load(const std::string&);
      void load_from_file(const std::string&);
      const std::vector<std::string>& get_marker_fields(const std::string& className, const std::string& key) ;
      
      static const std::bitset<4> METHOD_ENTER;
      static const std::bitset<4> METHOD_EXIT;
      static const std::bitset<4> METHOD_TIMER;
    };
  }
}

#endif
