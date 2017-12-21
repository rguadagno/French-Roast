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

#ifndef RPT_H
#define RPT_H
#include <string>
#include "OpCode.h"
#include "fr_signals.h"

namespace frenchroast { namespace agent {

    /*
    //        Example config file
    
opcodefile  <c:\richg\code\french_roast\French-Roast\modules\instrumentation\config\opcodes.txt>
hooksfile   <c:\richg\code\french_roast\French-Roast\modules\agent\hooks_config3.txt>
server             <127.0.0.1:6060>
reporter_descriptor    <server>
command_descriptor     <server>


    
    */
    
    class Config {
      std::string _reporterDescriptor{""};
      std::string _opcodeFile{""};
      std::string _hooksFile{""};
      std::string _server{""};
      std::string _serverip{""};
      int          _serverPort{6060};
      
    public:
      Config();
      bool load(std::string, frenchroast::OpCode&, frenchroast::signal::Signals&);
      std::string get_opcode_file() const;
      std::string get_hooks_file() const;
      const std::string& get_server_ip() const;
      int get_server_port() const;
      std::string get_report_filename() const;
    };
  }
}



#endif
