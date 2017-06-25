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

#ifndef FRMON_H
#define FRMON_H
#include <unordered_map>
#include "Connector.h"
#include "Listener.h"
#include "Util.h"
#include "StackTrace.h"
#include "MarkerField.h"


namespace frenchroast { namespace monitor {

    
    std::string translate_descriptor(const std::string& name);
    std::vector<StackTrace> construct_traffic(const std::string& msg);

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
        std::unordered_map<std::string, std::unordered_map<std::string, int>>         _markers;
        
	const int MSG_TYPE    = 0;
	const int MSG         = 1;
        const int MARKER      = 3;

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
	      _handler.signal_timed( translate_descriptor(items[DESCRIPTOR]), items[THREAD_NAME] , _timed_signals[key]._elapsed, elapsed);
	    }
	    if (items[DIRECTION] == "enter") {
	      std::string key = items[DESCRIPTOR] + items[THREAD_NAME];
	      _timed_signals[key]._last = std::stoll(items[TIME]);
	    }
	  }
	  if (items[MSG_TYPE] == "signal") {
            std::vector<MarkerField> mfields;
            if(items[MARKER] != "") {
              ++((_markers[items[MSG]])[items[MARKER]]);
              for(auto& item : _markers[items[MSG]]) {
                mfields.emplace_back(item.first, item.second);
              }
            }
            _handler.signal(translate_descriptor(items[MSG]) , items[2] , ++_signals[items[MSG]], mfields );

	   }
	  if (items[MSG_TYPE] == "traffic") {
	    _handler.traffic( construct_traffic(items[MSG]));
	  }

	 if (items[MSG_TYPE] == "connected") {
	   _handler.connected(items[MSG]);
	 }
         
	 if (items[MSG_TYPE] == "unloaded") {
	   _handler.unloaded(items[MSG]);
	 }
         
      }

      void init_receiver(const std::string& ipAddr, int port)
      {
        _conn.wait_for_client_connection(ipAddr, port, this);
      }

      void watch_traffic(const int interval_millis)
      {
	_conn.send_message("watch_traffic~" + std::to_string(interval_millis));
      }

      void stop_watch_traffic()
      {
	_conn.send_message("stop_watch_traffic~" );
      }


    };
  }
}
#endif
