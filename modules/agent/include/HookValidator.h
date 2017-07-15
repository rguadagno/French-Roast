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

#ifndef HOOKVALID_H
#define HOOKVALID_H

#include <string>

namespace frenchroast { namespace agent {

  class HookValidatorStatus {
    std::string _msg;
    bool _valid{true};
    bool _isComment{false};
  public:
    explicit HookValidatorStatus(bool comment);
    explicit HookValidatorStatus(const char*  errorMsg);
    bool valid();
    bool is_comment();
    const std::string& msg();
    operator bool();
  };

  HookValidatorStatus validate_hook(const std::string& line);

}
}

#endif
