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
    
    SignalValidationStatus::SignalValidationStatus(const char* errorMsg) : _msg(errorMsg), _valid(false)
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
      std::string mheapStr;

      try {
        validate(scopy, classStr, methodStr, flagStr, fieldStr,  artifactStr, mheapStr);
      }
      catch(std::invalid_argument& e) { return SignalValidationStatus{e.what()}; }
      
      return SignalValidationStatus{false};
    }


    
    void SignalValidator::validate(const std::string& line, std::string& classStr, std::string& methodStr, std::string& flagStr,std::string& fieldStr, std::string& artifactStr, std::string& mheapStr)
    {
      if(try_monitor_match(line, classStr, mheapStr)) return;
      if(try_signal_match(line, classStr, methodStr, flagStr, fieldStr, artifactStr)) return;
      report_error(line);
    }


    bool SignalValidator::try_monitor_match(const std::string& line, std::string& classstr, std::string& mheapstr)
    {
      std::smatch sm;
      std::regex_match(line, sm, _minorRegex);
      if(sm.size() != 3) return false;
      classstr = sm[1].str();
      mheapstr = sm[2].str();
      return true;
    }

    bool SignalValidator::try_signal_match(const std::string& line, std::string& classStr, std::string& methodStr, std::string& flagStr, std::string& fieldStr, std::string& artifactStr)
    {
      std::smatch sm;
      std::regex_match(line, sm, _fullRegex);
      if(sm.size() < 4) return false;
      classStr = sm[1].str();
      methodStr = sm[2].str();
      flagStr = sm[3].str();
      if(sm.size() >= 5 && sm[4].str() != "") {
        fieldStr = sm[4].str();
      }
      if(sm.size() == 6 && sm[5].str() != "") {
        artifactStr = sm[5].str();
      }
      return true;
    }

    bool SignalValidator::report_error(const std::string& line)
    {
      std::smatch sm;
      std::regex_match(line,sm,_methodRegex);
      if(sm.size() == 4) {
        throw std::invalid_argument{"signal point bad/missing: " + sm[3].str()};
      }
      else {
        throw std::invalid_argument{"bad method descriptor: " + line};
      }
    }
    
  }
}
