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

#ifndef MON_H
#define MON_H
#include <unordered_map>
#include "Connector.h"
#include "Listener.h"
#include "Util.h"

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


	struct time_holder {
	  long      _elapsed;
	  long long _last;

	};
	
      template
      <typename T>
	class Monitor : public network::Listener {
	T& _handler;
	network::Connector  _conn;
	std::unordered_map<std::string, time_holder> _timed_signals;
	std::unordered_map<std::string, int>         _signals;
	const int MSG_TYPE    = 0;
	const int MSG         = 1;
	const int TIME        = 1;
	const int DIRECTION   = 2;
	const int DESCRIPTOR  = 3;
	const int THREAD_NAME = 4;

      public:
      Monitor(T& handler) : _handler(handler)
      {
      }
      
      void message(const std::string& msg)
      {
	std::vector<std::string> items = frenchroast::split(msg,"~");
	 if (items[MSG_TYPE] == "signaltimer") {
	   if (items[DIRECTION] == "exit") {
	     std::string key = items[DESCRIPTOR] + items[THREAD_NAME];
	     int elapsed = std::stoll(items[TIME]) - _timed_signals[key]._last;
	     _timed_signals[key]._elapsed += elapsed;
	     //	     _timed_signals.erase(key);
	     _handler.signal_timed( translate_descriptor(items[DESCRIPTOR])+ " [" + items[THREAD_NAME] +"]",_timed_signals[key]._elapsed, elapsed);
	   }
	   if (items[DIRECTION] == "enter") {
	     std::string key = items[DESCRIPTOR] + items[THREAD_NAME];
	     _timed_signals[key]._last = std::stoll(items[TIME]);
	   }
	 }
	 if (items[MSG_TYPE] == "signal") {
	   _handler.signal(translate_descriptor(items[MSG]), ++_signals[items[MSG]]);
	 }
	 if (items[MSG_TYPE] == "connected") {
	   _handler.connected(items[MSG]);
	 }
	 
      }

      void init_receiver(const std::string& ipAddr, int port)
      {
        _conn.init_receiver(ipAddr, port, this);
      }


    };
  }
}
#endif
