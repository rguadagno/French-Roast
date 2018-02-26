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
//    along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
//

#include "fr_signals.h"
#include "Util.h"
#include "Descriptor.h"

namespace frenchroast { namespace signal {

    Signal::Signal(const std::string& name, int lineno) : _line(lineno), _name(name)
    {
    }

    Signal::Signal(const std::string& name, std::bitset<4> flags, bool artifacts) : _name(name), _includeArtifacts(artifacts), _flags(flags)
    {
      if(_name == "*") {
        _all = true;
      }
    }

    int Signal::line_number() const
    {
      return _line;
    }

    std::string Signal::method_name() const
    {
      return _name;
    }

    std::bitset<4> Signal::flags() const
    {
      return _flags;
    }

    bool Signal::all() const
    {
      return _all;
    }

    bool Signal::artifacts() const
    {
      return _includeArtifacts;
    }
    
    bool Signal::monitor_heap() const
    {
      return (_flags & Signals::MONITOR_HEAP) == Signals::MONITOR_HEAP;
    }

    bool Signal::valid() const
    {
      return _valid;
    }

    void Signal::mark_invalid()
    {
      _valid = false;
    }
    // -------------------------


    const std::vector<std::string>& Signals::get_marker_fields(const std::string& className, const std::string& key) 
    {
      return _markerFields[className + key];
    }
    
    bool Signals::is_signal_class(const std::string& name) const
    {
      return _hlist.count(name) > 0;
    }

    bool Signals::is_monitor_heap_class(const std::string& name) 
    {
      return is_signal_class(name) && _hlist[name][0].monitor_heap();
    }
    
    std::vector<std::string> Signals::classes() const
    {
      std::vector<std::string> rv;
      for(auto& x : _hlist) {
        rv.push_back(x.first);
      }
      return rv;
    }

    
    std::unordered_map<std::string, std::string> Signals::_type_map {  {"int","I"},
                                                                     {"bool","Z"},
                                                                     {"void","V"},
                                                                     {"long","J"},
                                                                     {"byte","B"},
                                                                     {"char","C"},
                                                                     {"double","D"},
                                                                     {"float","F"},
                                                                     {"short","S"},
                                                                     {"",""}
                                                                  };
    

    std::string Signals::get_type(const std::string& rep)
    {
      if(_type_map.find(rep) == _type_map.end()) {
        throw std::invalid_argument{"bad type: " + rep};;
      }
      return  _type_map[rep];
    }

    std::vector<std::string> Signals::parse_token_types(const std::string& pstr)
    {
      std::vector<std::string> rv;
      std::string tstr = pstr;
      replace(tstr, '(');
      replace(tstr, ')');
      replace(tstr, '.','/');
      
      for(auto& param : split(tstr, ",")) {
        if(param.find("[]") != std::string::npos) {
          if(param.find("/") != std::string::npos) {
            rv.push_back("[L" + split(param,"[]")[0] + ";" );
          }
          else {
            rv.push_back("[" + get_type(split(param,"[]")[0])  );
          }
        }
        else {
          if(param.find("/") != std::string::npos) {
            rv.push_back("L" + param + ";" );
          }
          else {
            rv.push_back( get_type(param)  );
          }
        }
      }
      return rv;
    }

    
    std::string Signals::convert_name(const std::string& name)
    {
      if(name.find("*") != std::string::npos) {
        return "*";
      }
      if(name.find(":") == std::string::npos) {
        return "";
      }
      std::string rv;

      std::string nameStr   = split(name, ':')[0];
      std::string returnStr = split(name, ':')[2];
      std::string paramStr  = split(name, ':')[1];
      rv = nameStr + ":(";
      for(auto& token : parse_token_types(paramStr)) {
        rv.append(token);
      }
      rv.append(")");
      rv.append(parse_token_types(returnStr)[0]);
      replace(rv,'.','/');
      return rv;
    }



    void Signals::parse_flags(std::bitset<4>& flags, std::string str)
    {
      for(auto& x : split(str,"|")) {
        if ( x == "<ENTER>") {
          flags |= METHOD_ENTER;
        }
        else if (x == "<EXIT>") {
          flags |= METHOD_EXIT;
      }
        else if (x == "<TIMER>") {
          flags |= METHOD_TIMER;
        }
      }
    }
   
   void Signals::load_from_file(const std::string& filename)
   {
      std::ifstream infile;
      try {
        infile.open(filename);
        if(infile.fail())
           throw std::ios_base::failure("cannot open file: " + filename);
        std::string line;
        while (getline(infile,line))
        {
          load(line);
        }
        infile.close();
      }
      catch(std::ios_base::failure&) {
        throw std::ios_base::failure("cannot open file: " + filename);
      }
     
   }

                   
    bool Signals::load(const std::string& pline)
    {
      std::string line{pline};
      remove_blanks(line);
      if (line[0] == '#' || line == "") {
        return false;;
      }
      std::string classname;

      std::string methName;
      std::string flagStr;
      std::string fieldStr;
      std::string artifactStr;
      std::string mheapStr;
      _validator.validate(line,classname, methName,flagStr, fieldStr, artifactStr, mheapStr);
      std::bitset<4> flags;
      if(mheapStr == "<MONITOR:HEAP>") {
        flags = MONITOR_HEAP;
      }
      else {
        parse_flags(flags, flagStr);
      }
      std::vector<std::string> fields;
      for(auto& x : split(fieldStr, "][")) {
        replace(x, '[');
        replace(x, ']');
        if(x != "") {
          fields.push_back(x);
        }
      }
      _markerFields[classname + methName] = fields;
      replace(classname, '.', '/');
      methName = convert_name(methName);
      _hlist[classname].push_back(Signal{methName, flags, artifactStr.find("OFF") != std::string::npos ? false : true});
      return true;
    }

    std::vector<Signal>& Signals::operator[](const std::string& name) 
    {
      return _hlist[name];
    }
    
    std::vector<std::string> get_signals(Signals& sigs)
    {
      std::vector<std::string> rv;
      for(auto& cl : sigs.classes()) {
        for(auto& sig : sigs[cl]) {
          replace(cl,'/', '.');
          if(sig.all()) {
            rv.push_back(cl +  "::*");
          }
          else {
            rv.push_back(monitor::Descriptor{" " + cl + "::" + sig.method_name()});
          }
        }
      }
      return rv;
    }
    
    const std::bitset<4> frenchroast::signal::Signals::METHOD_ENTER{"0001"};
    const std::bitset<4> frenchroast::signal::Signals::METHOD_EXIT {"0010"};
    const std::bitset<4> frenchroast::signal::Signals::METHOD_TIMER{"0100"};
    const std::bitset<4> frenchroast::signal::Signals::MONITOR_HEAP{"1000"};


  }
}
