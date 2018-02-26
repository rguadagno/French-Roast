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

#ifndef INSTREPORT_H
#define INSTREPORT_H

#include <string>
#include <sstream>
#include "Command.h"
#include "Descriptor.h"
#include "fr_signals.h"

namespace frenchroast { namespace monitor {
    class InstrumentationReport  {
      friend InstrumentationReport& operator>>(const std::string& rep, InstrumentationReport& ref);
      std::string             _hostname;
      std::string             _pid;
      std::vector<Descriptor> _valid;
      std::vector<Descriptor> _invalid;

    public:

      InstrumentationReport(const std::string& hostname, const std::string& id,const std::string& class_name, const std::vector<signal::Signal>&);
      InstrumentationReport() = default;;
      const std::vector<Descriptor>& valid() const;
      const std::vector<Descriptor>& invalid() const;
      const std::string& hostname() const;
      const std::string& pid() const;
      static const std::string DESCRIPTOR_DELIM;
    };

    template <typename OutType>
      OutType& operator<<(OutType& out, const InstrumentationReport& ref)
      {
        out << command::INSTRUMENTATION << ref.hostname() << "<end-hostname>" << ref.pid() << "<end-pid>";
        for(auto& x : ref.valid()) {
          out << x << InstrumentationReport::DESCRIPTOR_DELIM;
        }
        out  << "<end-valid>";
        for(auto& x : ref.invalid()) {
          out << x << InstrumentationReport::DESCRIPTOR_DELIM;
        }

        return out;
      }

    InstrumentationReport& operator>>(const std::string& rep, InstrumentationReport& ref);
    InstrumentationReport& operator>>(const std::string& rep, InstrumentationReport&& ref);
  }
}
#endif
