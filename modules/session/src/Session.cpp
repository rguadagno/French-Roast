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
    
    std::vector<frenchroast::monitor::ClassDetail> Session::get_loaded_classes() const
    {
      return _loaded_classes;
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
      _persistor->store(fileName, _loaded_classes, _jammed);
      
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
      _persistor->load(fileName, _loaded_classes, _jammed);
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
