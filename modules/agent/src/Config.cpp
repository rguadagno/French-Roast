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
//    along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
//
#include <iostream>
#include <fstream>
#include "Config.h"
#include "Util.h"
#include "Listener.h"
#include "Connector.h"

namespace frenchroast { namespace agent {
    std::string get_value(const std::string& key, const std::string& value, const std::string& line)
    {
      std::string rv = value;
      if(frenchroast::split(line,'<')[0] ==  key) {
         rv = frenchroast::split(line,'<')[1];
         rv = rv.substr(0,rv.length()-1);
      }
      return rv;
    }
    
    Config::Config()
    {
    }
    
    std::string Config::get_opcode_file() const
    {
      return _opcodeFile;
    }

    std::string Config::get_hooks_file() const
    {
      return _hooksFile;
    }

    std::string Config::get_report_filename() const
    {
      return _reporterDescriptor.substr(std::string{"file:"}.size());
    }

template
<typename LineLoadable>
    class LocalListener : public frenchroast::network::Listener {
      LineLoadable&             _dest;
  frenchroast::network::Connector<>& _conn;
      
    public:
      LocalListener(LineLoadable& dest, frenchroast::network::Connector<>& conn) : _dest(dest), _conn(conn)
      {
      }
      
      void message(const std::string& pmsg)
      {
        std::string msg = frenchroast::split(pmsg,"~")[1];
        if(msg.find("<end>") != std::string::npos) {
          _conn.close_down();
      }
      else {
        _dest.load(msg);
      }
      }
    };


    const std::string& Config::get_server_ip() const
    {
      return _serverip;
    }

    int Config::get_server_port() const 
    {
      return _serverPort;
    }
    
    bool Config::load(std::string descriptor, frenchroast::OpCode& opcodes, frenchroast::signal::Signals& hooks)
      {

        if(descriptor.find("server:") != std::string::npos) {
          frenchroast::replace(descriptor, "server:", "");
          _serverip = frenchroast::split(descriptor,":")[0];
          _serverPort = atoi(frenchroast::split(descriptor,":")[1].c_str());
          frenchroast::network::Connector<> conn;
          conn.connect_to_server(_serverip, _serverPort);
          conn.send_message("transmit-opcodes");
          conn.blocked_listen(new LocalListener<OpCode>(opcodes, conn));
          conn.connect_to_server(_serverip, _serverPort);
          conn.send_message("transmit-hooks");
          conn.blocked_listen(new LocalListener<frenchroast::signal::Signals>(hooks, conn));
          return true;
        }


      
      std::ifstream inconfig;
      try {
        inconfig.open(descriptor);
        if(inconfig.fail()) {
           throw std::ios_base::failure("cannot open file: " + descriptor);
        }
        std::string line;
        while (getline(inconfig,line)) {
          frenchroast::remove_blanks(line);
          _opcodeFile         = get_value("opcodefile",         _opcodeFile,line);
          _hooksFile          = get_value("hooksfile",          _hooksFile, line);
          _reporterDescriptor = get_value("reporter_descriptor",_reporterDescriptor, line);
          _server             = get_value("server",             _server, line);
        }
        inconfig.close();
        if (_opcodeFile == "") {
          std::cout << "opcodefile not set in config file" << std::endl;
          return false;
        }

        if (_hooksFile == "") {
          std::cout << "hooksfile not set in config file" << std::endl;
          return false;
        }
        
        if (_reporterDescriptor == "") {
          std::cout << "reporter descriptor not set in config file" << std::endl;
          return false;
        }

        opcodes.load_from_file(_opcodeFile);
      }
      catch(std::ifstream::failure& ) {
        if(!inconfig.eof())
          throw std::ifstream::failure("cannot open file: " + descriptor);
      }
      return true;
    }
    
  }
}


