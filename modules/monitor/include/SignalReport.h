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

#ifndef SIGNALRPT_H
#define SIGNALRPT_H

#include <unordered_map>
#include "Signal.h"
#include "MarkerField.h"


namespace frenchroast { namespace monitor {
    class SignalReport  {
      Descriptor                                     _descriptor;
      std::string                                    _thread_name;
      std::size_t                                    _count{0};
      std::string                                    _key{""};
      std::unordered_map<std::string, StackReport>   _stacks;
      std::unordered_map<std::string, MarkerField>   _markers;
      std::vector<std::string>                       _arg_headers;
      std::vector<std::string>                       _instance_headers;
      std::vector<Signal>                            _signals;
      std::vector<std::string>                        build_instance_headers(const SignalMarkers& markers);

    public:
      SignalReport()=default;
      SignalReport&        operator+=(const Signal&);
      std::size_t          count() const;
      const std::string&   key() const;
      const std::unordered_map<std::string, StackReport>&  stacks() const;
      const std::unordered_map<std::string, MarkerField>& markers() const;
      const std::vector<std::string>&                     arg_headers() const;
      const std::vector<std::string>&                     instance_headers() const;
      const std::string&                                  thread_name() const;
      std::string                                         descriptor_name() const;
      const std::vector<Signal>&                          xsignals() const;
    };

    template <typename OutType>
      OutType& operator<<(OutType& out, const SignalReport& ref)
      {
        for(auto& sig : ref.xsignals()) {
          out << sig << "<end-signal>";
        }
        return out;
      }

    template <typename OutType>
      OutType& operator<<(OutType& out, const std::unordered_map<std::string,SignalReport>& ref)
      {
        if(ref.size() == 0) return out;
        for(auto x : ref) {
          out << x.second <<  "<end-signal-report>";
        }
        return out;
      }

    SignalReport& operator>>(const std::string& rep, SignalReport& ref);
    SignalReport& operator>>(const std::string& rep, SignalReport&& ref);
    std::vector<SignalReport>& operator>>(const std::string&, std::vector<SignalReport>& ref);
    
    
  }
}
#endif
