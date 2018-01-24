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

#ifndef SIGVALID_H
#define SIGVALID_H

#include <string>
#include <regex>

namespace frenchroast { namespace signal {
  class SignalValidationStatus {
    std::string _msg;
    bool _valid{true};
    bool _isComment{false};
  public:
    explicit SignalValidationStatus(bool comment);
    explicit SignalValidationStatus(const char*  errorMsg);
    bool valid();
    bool is_comment();
    const std::string& msg();
    operator bool();
  };

    
    class SignalValidator {
      std::regex  _fullRegex{"((?:[A-Za-z0-9_]+\\.)+[A-Za-z0-9]+)::([A-Za-z0-9_\\*]+(?::\\([a-z]*\\):[a-z]+){0,})(<ENTER>|<EXIT>|<TIMER>){1}((?:\\[[A-Za-z0-9_]+\\]){0,})((?:<ARTIFACTS:OFF>){0,})"};
      std::regex  _minorRegex{"((?:[A-Za-z0-9_]+\\.)+[A-Za-z0-9]+)::(?:(<MONITOR:HEAP>){1})"};
      std::regex  _methodRegex{"((?:[A-Za-z0-9_]+\\.)+[A-Za-z0-9]+)::([A-Za-z0-9_]+:\\([a-z]*\\):[a-z]+)(.*)"};

      bool try_monitor_match(const std::string& line, std::string& classstr,std::string& mheapstr);
      bool try_signal_match(const std::string& line, std::string& classStr, std::string& methodStr, std::string& flagStr, std::string& fieldStr, std::string& artifactStr);
      bool report_error(const std::string& line);
    public:
      void validate(const std::string& line, std::string& classstr,std::string& methodstr,std::string& flagstr,std::string& fieldstr, std::string& artifactStr, std::string& mheapStr);
      SignalValidationStatus validate_no_throw(const std::string& line);
    };

    
  }
}
#endif
    
