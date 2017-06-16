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
    
    std::string Config::get_reporter_descriptor() const
    {
      return _reporterDescriptor;
    }

    std::string Config::get_opcode_file() const
    {
      return _opcodeFile;
    }

    std::string Config::get_hooks_file() const
    {
      return _hooksFile;
    }

    std::string Config::get_server() const
    {
      return _server;
    }


    bool Config::is_server_required() const
    {
      return _serverRequired;
    }

    bool Config::is_cout_reporter() const
    {
      return _reporterDescriptor.find("cout") != std::string::npos;;
    }

    bool Config::is_file_reporter() const
    {
      return _reporterDescriptor.find("file:") != std::string::npos;;
    }

    bool Config::is_server_reporter() const
    {
      return _reporterDescriptor.find("server") != std::string::npos;;
    }

    std::string Config::get_report_filename() const
    {
      return _reporterDescriptor.substr(std::string{"file:"}.size());
    }



    
    bool Config::load(const std::string& filename)
    {
      std::ifstream inconfig;
      try {
        inconfig.open(filename);
        if(inconfig.fail()) {
           throw std::ios_base::failure("cannot open file: " + filename);
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
       std::cout << "server: " << _server << std::endl;       
        if(_server != "") {
          _serverRequired = true;
        }
      }
      catch(std::ifstream::failure& ) {
        if(!inconfig.eof())
          throw std::ifstream::failure("cannot open file: " + filename);
      }
      return true;
    }
    
  }
}


