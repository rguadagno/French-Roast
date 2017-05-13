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
#include <unordered_map>
#include <string>

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
    

    std::string translate_descriptor(const std::string& name)
    {
      std::string rv = name.substr(1);
      replace(rv,'/','.');
      //            
            std::string classname = split(rv,"::")[0];
            replace(classname,';');
            std::string methodname = split(split(rv,"::")[1],":")[0];
            std::string pstr = split(split(rv,"(")[1],")")[0];
            std::string rvstr = split(split(rv,")")[1],":")[0];

            std::string parms = "";
            int pos = 0;
            while(pos < pstr.length() ) {
              if (pstr[pos] == 'L') {
                int nextsemi = pstr.find(";",pos);
                parms += pstr.substr(pos+1,nextsemi-(pos+1)) + std::string(",");
                pos = nextsemi +2;
              }
              else {
                parms += _type_map[pstr[pos]] + std::string(",");
                ++pos;
              }
            }
            if(parms.length() > 1)
              parms.erase(parms.length()-1);
            
            if ( rvstr.length() == 1) {
              rvstr = _type_map[rvstr[0]];
            }
            else {
              rvstr = rvstr.substr(1);
              rvstr.erase(rvstr.length());
            }
              
            return classname + "::" + methodname + "(" + parms + "):" + rvstr;
          }


    std::vector<StackTrace> construct_traffic(const std::string& msg)
    {
      std::vector<StackTrace> rv;
      std::vector<std::string> items = split(msg, "%");
      for(auto& x : items) {
        StackTrace st{split(x,"^")[0]};
        //      for(auto& y : split(split(x,"^")[1],"#")) {
        std::vector<std::string> fitems = split(split(x,"^")[1],"#");
        for(int idx = fitems.size()-1;idx >= 0; idx--) {
          st.addFrame(StackFrame{translate_descriptor(fitems[idx]),fitems[idx]});
        }
        rv.push_back(st);
      }

      return rv;
    }


  }}
