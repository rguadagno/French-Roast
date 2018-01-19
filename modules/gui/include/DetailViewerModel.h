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

#ifndef DETAILVIEWERMODEL_H
#define DETAILVIEWERMODEL_H

#include <unordered_map>
#include "qtablewidget.h"
#include "StackReport.h"

namespace frenchroast {
  class DetailViewerModel  {
  QTableWidget*                                        _stackData;
  std::unordered_map<std::string, QTableWidgetItem*>&  _items;
  public:
    DetailViewerModel(QTableWidget* stackData, std::unordered_map<std::string, QTableWidgetItem*>&  items);
    void update_stack_view(const std::unordered_map<std::string,frenchroast::monitor::StackReport>& stacks);
  };
}
#endif
