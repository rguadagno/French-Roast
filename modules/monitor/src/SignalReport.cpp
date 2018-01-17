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
#include "SignalReport.h"
#include "Util.h"

namespace frenchroast { namespace monitor {
    SignalReport& SignalReport::operator+=(const Signal& ref)
    {
      if(_key != "" && _key != ref.key()) return *this;
      ++_count;
      if(_key == "") {
        _key = ref.key();
        _thread_name = ref.thread_name();
        _descriptor = ref.report().descriptors()[0].descriptor();
        std::string desc = ref.descriptor();
        for(auto& x : frenchroast::split(frenchroast::split(frenchroast::split(desc,")")[0], "(")[1], ",")) {
          _arg_headers.push_back(x);
        }
        _instance_headers = build_instance_headers(ref.markers());

      }
      _stacks[ref.report().key()] += ref.report();
      std::string mkey = ref.params().key() + ref.markers().key();
      _markers[mkey] += MarkerField{mkey,ref.params(), ref.markers()};

      return *this;
    }

    std::size_t SignalReport::count() const
    {
      return _count;
    }

    const std::unordered_map<std::string, StackReport>&  SignalReport::stacks() const 
    {
      return _stacks;
    }

    const std::unordered_map<std::string, MarkerField>& SignalReport::markers() const
    {
      return _markers;
    }

    const std::vector<std::string>& SignalReport::arg_headers() const
    {
      return _arg_headers;
    }

    const std::vector<std::string>& SignalReport::instance_headers() const
    {
      return _instance_headers;
    }

    
    const std::string& SignalReport::key() const
    {
      return _key;
    }
    
    const std::string& SignalReport::thread_name() const
    {
      return _thread_name;
    }

    std::string SignalReport::descriptor_name() const
    {
      return _descriptor.full_name();
    }
    
    std::vector<std::string> SignalReport::build_instance_headers(const SignalMarkers& markers)
    {
      std::vector<std::string> rv;
      for(auto& item : markers) {
        rv.push_back(item.label);
      }
      return rv;
    }

  }
}
