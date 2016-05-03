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
//    along with French-Roast.  If not, see <http://www.gnu.org/licenses/>.
//

#ifndef FRSTACKFRAME_H
#define FRSTACKFRAME_H

#include <vector>

namespace frenchroast { namespace monitor {

    class StackFrame {
      std::string _name;
      std::string _decorated_name;
    public:
    StackFrame(const std::string& name, const std::string& deco) : _name(name), _decorated_name(deco)
      {
      }

      std::string get_name()  const 
      {
        return _name;
      }

      std::string get_decorated_name()  const
      {
	return _decorated_name;
      }
    };

  }
}


#endif
