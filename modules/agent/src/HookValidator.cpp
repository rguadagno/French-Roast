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

#include "HookValidator.h"
#include "Util.h"

namespace frenchroast { namespace agent {
    
    HookValidatorStatus::HookValidatorStatus(bool comment) : _isComment(comment)
    {
    }
    
    HookValidatorStatus::HookValidatorStatus(const char* errorMsg) : _valid(false), _msg(errorMsg)
    {
    }

    bool HookValidatorStatus::valid()
    {
      return _valid;
    }

    const std::string& HookValidatorStatus::msg()
    {
      return _msg;
    }

    HookValidatorStatus::operator bool()
    {
      return _valid;
    }

    bool HookValidatorStatus::is_comment()
    {
      return _isComment;
    }

    HookValidatorStatus validate_hook(const std::string& line)
    {
      std::string scopy = line;
      frenchroast::remove_blanks(scopy);
      if(scopy == "" || scopy[0] == '#') { return HookValidatorStatus{true}; }

      if(scopy.find("<ENTER>") == std::string::npos &&
         scopy.find("<EXIT>") == std::string::npos  &&
         scopy.find("<TIMER>") == std::string::npos ) {
        return HookValidatorStatus{"No directives (<ENTER>, <EXIT>, or <TIMER>) found."};
      }

      if(scopy.find("::") == std::string::npos ) {
        return HookValidatorStatus{"Missing class specifier (e.g. SomeClass::methodA..."};
      }

      return HookValidatorStatus{false};
      
    }

}
}
