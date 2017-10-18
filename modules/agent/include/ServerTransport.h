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

#ifndef SERVERTRANS_H
#define SERVERTRANS_H

#include <string>
#include "Transport.h"
#include "Connector.h"

namespace frenchroast { namespace agent {

    class ServerTransport : public Transport {
      network::Connector<>& _conn;
    public:
      ServerTransport(network::Connector<>& conn);
      void out(const std::string& str);

    };
  }}
#endif
