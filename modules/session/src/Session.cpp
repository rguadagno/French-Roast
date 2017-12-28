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
    }
    
    bool Session::operator==(const Session& ref) const
    {
      return (_loaded_classes == ref._loaded_classes );
    }
    
    bool Session::operator!=(const Session& ref) const
    {
      return !(*this == ref);
    }

    void Session::update_class_viewer(const std::vector<frenchroast::monitor::ClassDetail>& details)
    {
      _loaded_classes.insert(_loaded_classes.end(), details.begin(), details.end());
    }
    
    std::vector<frenchroast::monitor::ClassDetail> Session::get_class_viewer() const
    {
      return _loaded_classes;
    }

    void Session::store(const std::string& fileName)
    {
      if(_persistor == nullptr) {
        throw std::invalid_argument("no Persistor set");
      }
      _persistor->store(fileName, _loaded_classes);
    }

    void Session::load(const std::string& fileName)
    {
      if(_persistor == nullptr) {
        throw std::invalid_argument("no Persistor set");
      }

      _persistor->load(fileName, _loaded_classes);
    }
    
    Session::~Session()
    {
      delete _persistor;
    }
    
  }
}
