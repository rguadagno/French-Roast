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

#include "QUtil.h"
#include "DetailViewerModel.h"

namespace frenchroast {

  DetailViewerModel::DetailViewerModel(QTableWidget* stackData, std::unordered_map<std::string, QTableWidgetItem*>&  items)
    : _stackData(stackData), _items(items)
  {
  }
  
  void DetailViewerModel::update_stack_view(const std::unordered_map<std::string,frenchroast::monitor::StackReport>& stacks)
  {
    for(auto& x : stacks) {
      
      if(_items.count(x.second.key()) == 1) {
        _items[x.second.key()]->setText(  QString::fromStdString(std::to_string(x.second.count())));
      }
      else {
        int row = _stackData->rowCount();
        _stackData->insertRow(row);
        QTableWidgetItem* item = createItem(x.second.count());
        _items[x.second.key()] = item;
        _stackData->setItem(row, 0, item);
        for(auto& frame : x.second.descriptors()) {
          QTableWidgetItem* item = createItem(frame, Qt::AlignLeft|Qt::AlignVCenter);
          _stackData->setItem(row,1, item);
          ++row;
          _stackData->insertRow(row);
        }
        _stackData->removeRow(row);
      }
      
    }
  
  }
}
