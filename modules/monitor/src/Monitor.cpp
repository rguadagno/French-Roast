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




#include "Util.h"
#include "StackTrace.h"
#include "MarkerField.h"
#include <unordered_map>
#include <string>
#include "Connector.h"
#include "MethodStats.h"
#include "ClassDetail.h"

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
          int nextsemi = tstr.find(";",pos);
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
    
    std::string translate_descriptor(const std::string& name)
    {
      std::string rv = name.substr(1);
      replace(rv,'/','.');
      std::string classname = split(rv,"::")[0];
      replace(classname,';');
      std::string methodname = split(split(rv,"::")[1],":")[0];
      std::string pstr = split(split(rv,"(")[1],")")[0];
      std::string rvstr = translate_return_type(split(split(rv,")")[1],":")[0]);
      std::string parms = translate_param_types(pstr);
      return classname + "::" + methodname + ":(" + parms + "):" + rvstr;
    }


    std::vector<StackTrace> construct_traffic(const std::string& msg, std::unordered_map<std::string, MethodStats>& counters)
    {
      std::vector<StackTrace> rv;
      std::vector<std::string> items = split(msg, "%");
      for(auto& x : items) {
        StackTrace st{split(x,"^")[0]};
        //      for(auto& y : split(split(x,"^")[1],"#")) {
        std::vector<std::string> fitems = split(split(x,"^")[1],"#");

        bool first=true;
        for(int idx = fitems.size()-1;idx >= 0; idx--) {
          if(fitems[idx].find("thook") == std::string::npos ) {
            std::string descriptor = translate_descriptor(fitems[idx]);
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
            st.addFrame(StackFrame{descriptor,fitems[idx]});
          }
        }
        rv.push_back(st);
      }
      return rv;
    }



    std::string translate_method(const std::string& pname)
    {
      std::string name = pname;
      replace(name,'/','.');
      std::string methodname = split(name, ":")[0];
      std::string rvstr = translate_return_type(split(split(name,")")[1],":")[0]);
      std::string parms = translate_param_types(split(split(name,"(")[1],")")[0]);
      return methodname + ":(" + parms + "):" + rvstr;
    }
    
    std::vector<ClassDetail> construct_class_details(const std::string& msg)
    {
      std::vector<ClassDetail> rv;
      for(auto& citem : split(msg, "]^")) {
        if(citem == "") break;
        std::string name = split(citem, "^[")[0];
        replace(name, "/", ".");
        std::vector<std::string> methods;
        for(auto& mitem : split(split(citem,"^[")[1],"%")) {
          if(mitem == "") break;
          methods.push_back(translate_method(mitem));
        }
        rv.emplace_back(name, methods);
      }
      
      return rv;
    }
    

    void transmit_lines(const std::string& fileName, frenchroast::network::Connector& conn)
    {
      try {
        std::ifstream in;
        in.open(fileName);
        std::string line;
        while (getline(in,line)) {
          conn.send_message(line);
        }
        in.close();
        conn.send_message("<end>");
      }
      catch(std::ifstream::failure& ) {
        throw std::ifstream::failure("cannot open file: " + fileName);
      }
      
    }

    void transmit_lines(const std::vector<std::string>& lines, frenchroast::network::Connector& conn)
    {
      for(auto& line : lines) {
        conn.send_message(line);
      }
      conn.send_message("<end>");
    }
    
    
  }}
