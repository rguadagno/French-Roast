// copyright (c) 2018 Richard Guadagno
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


#include <iostream>
#include "Signal.h"
#include "Util.h"

namespace frenchroast { namespace monitor {


    Signal::Signal(const StackReport& sreport, const SignalParams& sparams, const SignalMarkers smarkers) :
      _report(sreport), _params(sparams), _markers(smarkers)
    {
    }
    
    const std::string Signal::thread_name() const
    {
      return  _report.trace().thread_name();
    }

    const std::string Signal::key() const
    {
      return thread_name() + descriptor();
    }
    
    const std::size_t Signal::count() const
    {
      return _report.count();
    }
    
    const std::string Signal::descriptor() const
    {
      return _report.trace().descriptor_frames()[0].get_name();
    }


  }
}
