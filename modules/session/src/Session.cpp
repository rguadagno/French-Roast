// copyright (c) 2017 Richard Guadagno
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

#include <stdexcept>
#include <sstream>
#include "Session.h"
#include "Command.h"

namespace frenchroast { namespace session {
    Session::Session(Persistor* persistor) :  _persistor(persistor) {}
    Session::Session(Persistor* persistor, const std::string& descriptor, std::size_t seconds) : _descriptor(descriptor), _seconds(seconds), _persistor(persistor)
    {
    }
    
    Session::Session(const Session& ref)
    {
      _loaded_classes = ref._loaded_classes;
      _jammed = ref._jammed;
    }
    
    bool Session::operator==(const Session& ref) const
    {
      return (_loaded_classes == ref._loaded_classes && _jammed == ref._jammed);
    }
    
    bool Session::operator!=(const Session& ref) const
    {
      return !(*this == ref);
    }

    void Session::update(const std::vector<frenchroast::monitor::ClassDetail>& details)
    {
      _loaded_classes.insert(_loaded_classes.end(), details.begin(), details.end());
    }

    void Session::update(const frenchroast::monitor::JammedReport& rpt)
    {
      if(_jammed.count(rpt.key()) == 0) {
        _jammed[rpt.key()] = rpt;
      }
      else {
        _jammed[rpt.key()] += rpt;
      }
    }

    void Session::update(const std::vector<frenchroast::monitor::StackTrace>& traffic)
    {
      _traffic.insert(_traffic.end(),traffic.begin(),traffic.end());
    }

    void Session::update(const std::vector<frenchroast::monitor::MethodStats>& methods)
    {
      _method_rankings = methods;
    }
    
    void Session::update(const frenchroast::monitor::SignalReport& rpt)
    {
      _signals[rpt.key()] = rpt;
    }

    void Session::update(const frenchroast::monitor::TimerReport& rpt)
    {
      _timers[rpt.key()] = rpt;
    }
    
    std::vector<frenchroast::monitor::StackTrace> Session::get_traffic() const
    {
      return _traffic;
    }
    
    std::vector<frenchroast::monitor::ClassDetail> Session::get_loaded_classes() const
    {
      return _loaded_classes;
    }

    std::vector<frenchroast::monitor::MethodStats> Session::get_rankings() const
    {
      return _method_rankings;
    }

    std::unordered_map<std::string, frenchroast::monitor::JammedReport> Session::get_jammed_reports() const
    {
      return _jammed;
    }

    std::unordered_map<std::string, frenchroast::monitor::SignalReport> Session::get_signal_reports() const
    {
      return _signals;
    }

    std::unordered_map<std::string, frenchroast::monitor::TimerReport>  Session::get_timer_reports() const
    {
      return _timers;
    }
    
    void Session::store(const std::string& fileName)
    {
      if(_persistor == nullptr) {
        throw std::invalid_argument("no Persistor set");
      }

      _descriptor = fileName;
      std::vector<std::string> items;
      std::stringstream serial;
      items.push_back((serial << _jammed).str());
      serial.str("");
      items.push_back((serial << _loaded_classes).str());
      serial.str("");
      serial << frenchroast::monitor::command::TRAFFIC;
      items.push_back((serial << _traffic).str());
      serial.str("");
      items.push_back((serial << _method_rankings).str());
      serial.str("");
      serial << frenchroast::monitor::command::SIGNAL_REPORTS;
      items.push_back((serial << _signals).str());
      serial.str("");
      items.push_back((serial << _timers).str());
      _persistor->store(fileName, items);
    }


    
    void Session::store()
    {
      if(!has_descriptor()) throw std::invalid_argument("no storage descriptor available");
      store(_descriptor);
    }
    
    void Session::load(const std::string& fileName)
    {
      if(_persistor == nullptr) {
        throw std::invalid_argument("no Persistor set");
      }
      reset();
      std::vector<frenchroast::monitor::JammedReport> jams;
      std::vector<frenchroast::monitor::SignalReport> sigs;
      std::vector<frenchroast::monitor::TimerReport> timers;
      using namespace monitor;
      _persistor->load(fileName);
      _persistor->load(command::LOADED, _loaded_classes);
      _persistor->load(command::JAMMED, jams);
      _persistor->load(command::TRAFFIC, _traffic);
      _persistor->load(command::METHOD_STATS, _method_rankings);
      _persistor->load(command::SIGNAL_REPORTS, sigs);
      _persistor->load(command::SIGNAL_TIMER, timers);
      for(auto& jam : jams) {
       _jammed[jam.key()] = jam;
      }
      for(auto& sig : sigs) {
        _signals[sig.key()] = sig;
      }
      for(auto& timer : timers) {
        _timers[timer.key()] = timer;
      }
      
    }

    void Session::reset()
    {
      _loaded_classes.clear();
    }
    
    bool Session::has_descriptor() const
    {
      return _descriptor != "";
    }
    
    Session::~Session()
    {
      delete _persistor;
    }
    
  }
}
