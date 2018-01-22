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

#include <QTableWidget>
#include "QUtil.h"
#include "JammedViewerModel.h"

namespace frenchroast {


  JammedViewerModel::JammedViewerModel(QTableWidget* ptr,std::unordered_map<std::string, int>& jamsRow, std::unordered_map<std::string, int>&  jamsCount) : _data(ptr),_jamsRow(jamsRow), _jamsCount(jamsCount)
  {
  }

  void JammedViewerModel::update(const frenchroast::monitor::JammedReport& rpt)
  {
    if(_jamsRow.count(rpt.key()) == 0) {
      if(_data->rowCount() > 0) {
        _data->insertRow(_data->rowCount());
      }
      _jamsRow[rpt.key()] = _data->rowCount();
      std::size_t newrows = rpt.waiter().size() > rpt.owner().size() ? rpt.waiter().size() : rpt.owner().size();
      newrows = rpt.monitors().size() > newrows ? rpt.monitors().size() : newrows;
      for(std::size_t idx = 0; idx < newrows;idx++) {
        _data->insertRow(_data->rowCount());
      }
      
      int start = _jamsRow[rpt.key()];
      for(auto& x : rpt.waiter().descriptor_frames()) {
        _data->setItem(start++,COL_WAITER, createItem(x));
      }

      start = _jamsRow[rpt.key()];
      for(auto& x : rpt.owner().descriptor_frames()) {
        _data->setItem(start++,COL_OWNER, createItem(x));
      }
      
      start = _jamsRow[rpt.key()];
      for(auto& x : rpt.monitors()) {
        _data->setItem(start++,COL_MONITORS, createItem(x));
      }
    }

    _data->setItem(_jamsRow[rpt.key()], COL_COUNT, createItem(++_jamsCount[rpt.key()]));

  }

  const int    JammedViewerModel::COL_COUNT    = 0;
  const int    JammedViewerModel::COL_MONITORS = 1;
  const int    JammedViewerModel::COL_WAITER   = 2;
  const int    JammedViewerModel::COL_OWNER    = 3;

}
