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

#include <unordered_map>
#include <string>
#include "Util.h"
#include "StackTrace.h"
#include "MarkerField.h"
#include "Connector.h"
#include "MethodStats.h"
#include "StackFrame.h"
#include "JammedReport.h"
#include "Descriptor.h"

namespace frenchroast { namespace monitor {

    
    std::vector<StackTrace> construct_traffic(const std::string& msg, std::unordered_map<std::string, MethodStats>& counters)
    {
      std::vector<StackTrace> rv;
      std::vector<std::string> items = split(msg.substr(1), "%");
      for(auto& x : items) {
        StackTrace st{split(x,"^")[0]};
        std::vector<std::string> fitems = split(split(x,"^")[1],"#");

        bool first=true;
        for(int idx = static_cast<int>(fitems.size())-1;idx >= 0; idx--) {
          if(fitems[idx].find("thook") == std::string::npos ) {
            Descriptor dsc{frenchroast::split(fitems[idx], "!")[1]};
            std::string descriptor = dsc.full_name();
            int moncount = atoi(frenchroast::split(fitems[idx], "!")[0].c_str());
            if(fitems[idx].find("java") == std::string::npos && fitems[idx].find("sun") == std::string::npos)  {
              if(counters.count(descriptor) == 0 ) {
                counters[descriptor] = MethodStats(descriptor);
              }
              ++counters[descriptor];
              if(first) {
                ++counters[descriptor];
                first = false;
              }
            }
            st.addFrame(StackFrame{dsc,moncount});
          }
        }
        rv.push_back(st);
      }
      return rv;
    }

    void transmit_lines(const std::string& fileName, const std::string& ipport, frenchroast::network::Connector<>& conn)
    {
      try {
        std::ifstream in;
        in.open(fileName);
        std::string line;
        while (getline(in,line)) {
          conn.send_message(ipport,line);
        }
        in.close();
        conn.send_message(ipport,"<end>");
      }
      catch(std::ifstream::failure& ) {
        throw std::ifstream::failure("cannot open file: " + fileName);
      }
      
    }

    void transmit_lines(const std::vector<std::string>& lines,  const std::string& ipport, frenchroast::network::Connector<>& conn)
    {
      for(auto& line : lines) {
        conn.send_message(ipport, line );
      }
      conn.send_message(ipport, "<end>");
    }
  }
}
