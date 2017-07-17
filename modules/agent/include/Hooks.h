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

#ifndef HOOKS_H
#define HOOKS_H

#include <unordered_map>
#include <vector>
#include <unordered_map>
#include <bitset>

namespace frenchroast { namespace agent {
    class Hook {
      int            _line;
      std::string    _name;
      std::bitset<4> _flags;
      bool           _all{false};
    public:
      Hook(const std::string& name, int lineno);
      Hook(const std::string& name, std::bitset<4>);
      int            line_number() const;
      std::string    method_name() const;
      std::bitset<4> flags() const;
      bool all() const;
    };
    
    class Hooks {
      static std::unordered_map<std::string, std::string>      _type_map;
      std::unordered_map<std::string,std::vector<Hook>>        _hlist;
      std::unordered_map<std::string,std::vector<std::string>> _markerFields;
      
      void validate(const std::string& method);
      void convert_name(std::string& name);

    public:
      bool is_hook_class(const std::string& name) const;
      const std::vector<Hook>& get(const std::string& name);
      void load(const std::string&);
      void load_from_file(const std::string&);
      const std::vector<std::string>& get_marker_fields(const std::string& className, const std::string& key) ;
    };
  }
}

#endif
