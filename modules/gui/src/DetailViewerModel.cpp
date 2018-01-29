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

  DetailViewerModel::DetailViewerModel(QTableWidget* stackData, QTableWidget* argData, std::unordered_map<std::string, QTableWidgetItem*>&  items)
    : _stackData(stackData), _argData(argData),_items(items)
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

  void DetailViewerModel::init_arg_instance_headers(const std::vector<std::string>& argHeaders, const std::vector<std::string>& instanceHeaders)
  {
    if(_argData->rowCount() > 0) return;
    
    _argData->insertColumn(0);
    _argData->insertColumn(0);
    _argData->setHorizontalHeaderItem(0, createItem("invoked"));
    _argData->setHorizontalHeaderItem(1,createItem("("));
    
    int colidx = 2;
    for(auto& x : argHeaders) {
      _argData->insertColumn(colidx);
      _argData->setHorizontalHeaderItem(colidx++,createItem(x));
    }

    _argData->insertColumn(colidx);
    _argData->setHorizontalHeaderItem(colidx++,createItem(")"));
    
    for(auto& x : instanceHeaders) {
      _argData->insertColumn(colidx);
      _argData->setHorizontalHeaderItem(colidx++,createItem(x));
    }
  }

  void DetailViewerModel::update_args_markers(const std::unordered_map<std::string,frenchroast::monitor::MarkerField>& markers)
  {
    for(auto& xitem : markers ) {
      auto& item = xitem.second;
      if(_detailItems.count(xitem.first) == 0 ) {
        if(item._count > 1) {
          int currRow = _argData->rowCount();
          _argData->insertRow(currRow);
          _detailItems[xitem.first] = currRow;
          _argData->setItem(currRow, 0, createItem(item._count));
          _argData->setItem(currRow, 1, createItem("("));
      
          int colidx = 2;
          std::size_t idx=1;
          auto totalargs = item._arg_items.size();
          for(auto& x : item._arg_items) {
            std::string xx = x;
            if(idx < totalargs) {
              xx.append(",");
              ++idx;
            }
            _argData->setItem(currRow, colidx++, createItem(xx));
          }
          
          _argData->setItem(currRow, colidx, createItem(")"));
          ++colidx;
          for(auto& x : item._instance_items) {
            _argData->setItem(currRow, colidx++,createItem(x));
          }
        }
        else {
          _detailItems[xitem.first] = -1;
          int currRow = _argData->rowCount();
          if( _detailItems.count("*") == 0) {
            _argData->insertRow(currRow);
            _detailItems["*"] = currRow;
            _argData->setItem(currRow, 0, createItem(1));
            _argData->setItem(currRow, 1, createItem("*"));
          }
          else {
            _argData->setItem(_detailItems["*"], 0, createItem(  _argData->item(_detailItems["*"],0)->text().toInt() + 1   ));
          }
        }
      }
      else {
        if(_detailItems[xitem.first] == -1  ) {
          if(item._count == 1) continue;
          QTableWidgetItem* titem = _argData->item(_detailItems["*"], 0);
          int total = titem->text().toInt() - 1;
          titem->setText( QString::number(total));
          int currRow = _argData->rowCount();
          _argData->insertRow(currRow);
          _detailItems[xitem.first] = currRow;
          _argData->setItem(currRow, 0, createItem(item._count));
          _argData->setItem(currRow, 1, createItem("("));
          int colidx = 2;
          std::size_t idx=1;
          auto totalargs = item._arg_items.size();
          for(auto& x : item._arg_items) {
            std::string xx = x;
            if(idx < totalargs) {
              xx.append(",");
              ++idx;
            }
            _argData->setItem(currRow, colidx++, createItem(xx));
          }
          
      _argData->setItem(currRow, colidx, createItem(")"));
      ++colidx;
      for(auto& x : item._instance_items) {
        _argData->setItem(currRow, colidx++,createItem(x));
      }
    }
    else {
      _argData->item(_detailItems[xitem.first],0)->setText(  QString::fromStdString(std::to_string(item._count)) );
    }
  }
  }


    
  }

  
}
