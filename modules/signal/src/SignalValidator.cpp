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

#include "SignalValidator.h"
#include "Util.h"

namespace frenchroast { namespace signal {

    SignalValidationStatus::SignalValidationStatus(bool comment) : _isComment(comment)
    {
    }
    
    SignalValidationStatus::SignalValidationStatus(const char* errorMsg) : _valid(false), _msg(errorMsg)
    {
    }

    bool SignalValidationStatus::valid()
    {
      return _valid;
    }

    const std::string& SignalValidationStatus::msg()
    {
      return _msg;
    }

    SignalValidationStatus::operator bool()
    {
      return _valid;
    }

    bool SignalValidationStatus::is_comment()
    {
      return _isComment;
    }

    SignalValidationStatus SignalValidator::validate_no_throw(const std::string& line)
    {
      std::string scopy = line;
      remove_blanks(scopy);
      if(scopy == "" || scopy[0] == '#') {
        return SignalValidationStatus{true};
      }

      std::string classStr;
      std::string methodStr;
      std::string flagStr;
      std::string fieldStr;
      std::string artifactStr;

      try {
        validate(scopy, classStr, methodStr, flagStr, fieldStr,  artifactStr);
      }
      catch(std::invalid_argument& e) { return SignalValidationStatus{e.what()}; }
      
      return SignalValidationStatus{false};
    }


    
    void SignalValidator::validate(const std::string& method, std::string& classStr, std::string& methodStr, std::string& flagStr,std::string& fieldStr, std::string& artifactStr)
    {
      std::smatch sm;
      std::regex_match(method,sm,_fullRegex);
      if(sm.size() < 4 ) {
        std::regex_match(method,sm,_methodRegex);
        if(sm.size() == 4) {
          throw std::invalid_argument{"signal point bad/missing: " + sm[3].str()};
        }
        else {
          throw std::invalid_argument{"bad method descriptor: " + method};
        }
      }
      classStr = sm[1].str();
      methodStr = sm[2].str();
      flagStr = sm[3].str();
      if(sm.size() >= 5 && sm[4].str() != "") {
        fieldStr = sm[4].str();
      }
      if(sm.size() == 6 && sm[5].str() != "") {
        artifactStr = sm[5].str();
      }

    }


  }

}
