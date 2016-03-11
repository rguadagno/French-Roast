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
//    along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
//

#include "Hooks.h"
#include "FrenchRoast.h"

namespace frenchroast { namespace monitoring {

    Hook::Hook(const std::string& name, int lineno) : _name(name), _line(lineno)
    {
    }

    Hook::Hook(const std::string& name, std::bitset<4> flags) : _name(name), _flags(flags), _line(0)
    {
    }
    
    int Hook::line_number() const
    {
      return _line;
    }

    std::string Hook::method_name() const
    {
      return _name;
    }

    std::bitset<4> Hook::flags() const
    {
      return frenchroast::FrenchRoast::METHOD_ENTER;
    }

    // -------------------------
    
    bool Hooks::is_hook_class(const std::string& name) const
    {
      return _hlist.count(name) > 0;
    }

    std::unordered_map<std::string, std::string> Hooks::_type_map { {"int","I"},{"bool","Z"},{"void","V"}};
    
    void Hooks::convert_name(std::string& name)
    {
      if (name.find("(") != std::string::npos) { // is method
	std::string n1 = split(name, ':')[0];
	std::string n2 = split(name, ':')[2];
	std::string params = split(name, ':')[1];
	name = n1 + ":(";
	std::vector<std::string> plist  = split(params.substr(1,params.length()-2));
	for (auto& x : plist) {
	  if (x.find(".") != std::string::npos) {
	    x = "L" + x + ";";
	  }
	  else {
	    x = _type_map[x];
	  }
	  replace(x,'.','/');
	  name += x;
	}
	name += ")";
	if (n2.find(".") != std::string::npos) {
	  convert_name(n2);
	  name += "L" + n2 + ";";
	}
	else {
	  name +=  _type_map[n2];
	}
      }
      else {
	replace(name,'.','/');
      }
    }
    
    void Hooks::load(const std::string& filename)
    {
      std::ifstream infile;
      try {
	infile.open(filename);
	bool loaded = false;
	std::string line;
	while (getline(infile,line))
	{
	  validate(line);
	  std::string classname = split(line, "::")[0];
	  convert_name(classname);
	  std::string methName = split(split(line, '<')[0],"::")[1];
          remove_blanks(methName);
  	  convert_name(methName);
	  _hlist[classname].push_back(Hook{methName, frenchroast::FrenchRoast::METHOD_ENTER});
	  loaded = true;
	}
        infile.close();
	if(!loaded) {
	  throw std::invalid_argument{"hooks file empty"};
	}
      }
      catch(std::ifstream::failure& e) {
	throw std::ifstream::failure("cannot open file: " + filename);
      }
    }

    const std::vector<Hook>& Hooks::get(const std::string& name)
    {
      return _hlist[name];
    }
    
    void Hooks::validate(const std::string& method)
    {
      if (method.find("::") == std::string::npos) {
	throw std::invalid_argument{"bad method name: " + method};
      }
      
      if (method.find(":(") == std::string::npos) {
	throw std::invalid_argument{"bad method name: " + method};
      }
      
      if (method.find("<") == std::string::npos) {
	throw std::invalid_argument{"bad method name: " + method};
      }
      
    }

  }
}
