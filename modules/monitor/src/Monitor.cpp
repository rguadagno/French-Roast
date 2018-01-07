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

            std::unordered_map<char, std::string> _type_map { {'I',"int"},
                                                             {'Z',"bool"},
                                                             {'V',"void"},
                                                             {'J',"long"},
                                                             {'B',"byte"},
                                                             {'C',"char"},
                                                             {'D',"double"},
                                                             {'F',"float"},
                                                             {'S',"short"}
                
                                                           };
    MarkerField build_marker( std::string str)
    {
      MarkerField mf{str};
      
      for(auto& x: frenchroast::split(frenchroast::split(str, ")")[0].substr(1),",")) {
        mf._arg_items.push_back(x);
      }
      
      for(auto& x: frenchroast::split(frenchroast::split(str, ")")[1],";")) {
        if(x.find(":") != std::string::npos) {
          mf._instance_items.push_back(frenchroast::split(x,":")[1]);
        }
      }
      
      return mf;
    }
    
    std::vector<std::string> build_instance_headers(const std::string& subkey)
    {
      std::vector<std::string> rv;
      for(auto& x: frenchroast::split(frenchroast::split(subkey, ")")[1],";")) {
        if(x.find(":") != std::string::npos) {
          rv.push_back(frenchroast::split(x,":")[0]);
        }
      }
      return rv;
    }
    
    std::vector<std::string> parse_type_tokens(const std::string& tstr)
    {
      std::vector<std::string> rv;
      int pos = 0;

      while(pos < tstr.length() ) {
        std::string suffix = "";
        if(tstr[pos] == '[') {
          suffix = "[]";
          ++pos;
        }
        if (tstr[pos] == 'L') {
          int nextsemi = static_cast<int>(tstr.find(";",pos));
          rv.push_back(tstr.substr(pos+1,nextsemi-(pos+1)) + suffix );
          pos = nextsemi +1;
        }
        else {
          rv.push_back(_type_map[tstr[pos]] + suffix);
          ++pos;
        }
      }
      return rv;
    }


    
    std::string translate_param_types(const std::string& pstr)
    {
      std::string rv = "";

      for(auto& token : parse_type_tokens(pstr)) {
        rv.append(token);
        rv.append(",");
      }
      if(rv.length() > 1) {
        rv.erase(rv.length()-1);
      }
      return rv;
    }

    std::string translate_return_type(const std::string& name)
    {
      return parse_type_tokens(name)[0];
    }
    
    std::vector<StackTrace> construct_traffic(const std::string& msg, std::unordered_map<std::string, MethodStats>& counters)
    {
      std::vector<StackTrace> rv;
      std::vector<std::string> items = split(msg, "%");
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



    StackTrace build_trace(const std::string& str)
    {
      StackTrace rv{};
      std::vector<std::string> funcs = split(str, "#");
      for(int idx = static_cast<int>(funcs.size())-2;idx >= 0; idx--) {
        rv.addFrame(StackFrame{Descriptor{funcs[idx]}});
      }
      return rv;
    }
    
    JammedReport& process_jammed(const std::string& rep, std::unordered_map<std::string, JammedReport>& reports)
    {
      JammedReport rpt{};
      rep >> rpt;
      if(reports.count(rep) == 0) {
        reports[rep] = rpt;
        return reports[rep];
      }
      return reports[rep] += rpt;
    }




    
  }}
