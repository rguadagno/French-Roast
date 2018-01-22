// copyright (c) 2018 Richard Guadagno
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

#ifndef JAMMEDVIEWERMODEL_H
#define JAMMEDVIEWERMODEL_H

#include "qtablewidget.h"
#include "JammedReport.h"

namespace frenchroast {
  class JammedViewerModel  {
    QTableWidget*                          _data;
    std::unordered_map<std::string, int>&  _jamsRow;
    std::unordered_map<std::string, int>&  _jamsCount;

  public:
    JammedViewerModel(QTableWidget*, std::unordered_map<std::string, int>& jamsRow, std::unordered_map<std::string, int>&  jamsCount);
    void update(const frenchroast::monitor::JammedReport&);

    static const int    COL_COUNT;
    static const int    COL_MONITORS;
    static const int    COL_WAITER;
    static const int    COL_OWNER;
  };
}

#endif
