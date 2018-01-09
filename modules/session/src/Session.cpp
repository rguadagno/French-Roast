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

#include <stdexcept>
#include <sstream>
#include "Session.h"

namespace frenchroast { namespace session {
    Session::Session(Persistor* persistor) :  _persistor(persistor) {}
    Session::Session(Persistor* persistor, const std::string& descriptor, std::size_t seconds) : _descriptor(descriptor), _seconds(seconds), _persistor(persistor)
    {
    }
    
    Session::Session(const Session& ref)
    {
      _loaded_classes = ref._loaded_classes;
      _jammed = ref._jammed;
    }
    
    bool Session::operator==(const Session& ref) const
    {
      return (_loaded_classes == ref._loaded_classes && _jammed == ref._jammed);
    }
    
    bool Session::operator!=(const Session& ref) const
    {
      return !(*this == ref);
    }

    void Session::update(const std::vector<frenchroast::monitor::ClassDetail>& details)
    {
      _loaded_classes.insert(_loaded_classes.end(), details.begin(), details.end());
    }

    void Session::update(const frenchroast::monitor::JammedReport& rpt)
    {
      if(_jammed.count(rpt.key()) == 0) {
        _jammed[rpt.key()] = rpt;
      }
      else {
        _jammed[rpt.key()] += rpt;
      }
    }

    void Session::update(const std::vector<frenchroast::monitor::StackTrace>& traffic)
    {
      _traffic.insert(_traffic.end(),traffic.begin(),traffic.end());
    }

    void Session::update(const std::vector<frenchroast::monitor::MethodStats>& methods)
    {
      _method_rankings = methods;
    }

    std::vector<frenchroast::monitor::StackTrace> Session::get_traffic() const
    {
      return _traffic;
    }
    
    std::vector<frenchroast::monitor::ClassDetail> Session::get_loaded_classes() const
    {
      return _loaded_classes;
    }

    std::vector<frenchroast::monitor::MethodStats> Session::get_rankings() const
    {
      return _method_rankings;
    }

    std::unordered_map<std::string, frenchroast::monitor::JammedReport> Session::get_jammed_reports() const
    {
      return _jammed;
    }
    
    void Session::store(const std::string& fileName)
    {
      if(_persistor == nullptr) {
        throw std::invalid_argument("no Persistor set");
      }

      _descriptor = fileName;
      std::vector<std::string> items;
      std::stringstream serial;
      items.push_back((serial << _jammed).str());
      serial.str("");
      items.push_back((serial << _loaded_classes).str());
      serial.str("");
      serial << "<traffic><view>";
      items.push_back((serial << _traffic).str());
      serial.str("");
      items.push_back((serial << _method_rankings).str());
      _persistor->store(fileName, items);
    }


    
    void Session::store()
    {
      if(!has_descriptor()) throw std::invalid_argument("no storage descriptor available");
      store(_descriptor);
    }
    
    void Session::load(const std::string& fileName)
    {
      if(_persistor == nullptr) {
        throw std::invalid_argument("no Persistor set");
      }
      reset();
      std::vector<frenchroast::monitor::JammedReport> jams;
      _persistor->load(fileName);
      _persistor->load(frenchroast::monitor::ClassDetail::TAG, _loaded_classes);
      _persistor->load(frenchroast::monitor::JammedReport::TAG, jams);
      _persistor->load("<traffic>", _traffic);
      _persistor->load(frenchroast::monitor::MethodStats::TAG, _method_rankings);
      for(auto& jam : jams) {
       _jammed[jam.key()] = jam;
      }
      
    }

    void Session::reset()
    {
      _loaded_classes.clear();
    }
    
    bool Session::has_descriptor() const
    {
      return _descriptor != "";
    }
    
    Session::~Session()
    {
      delete _persistor;
    }
    
  }
}
