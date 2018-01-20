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

#include <iostream>
#include <QHeaderView>
#include <QVBoxLayout>
#include "QUtil.h"
#include "SignalDelegate.h"
#include "DetailViewer.h"
#include "KeyListener.h"

namespace frenchroast {

  DetailViewer* DetailViewer::instance(QWidget* parent, const std::string& descriptor)
  {
    DetailViewer* rv = new DetailViewer(parent, descriptor);
    dynamic_cast<QMainWindow*>(parent)->addDockWidget(Qt::TopDockWidgetArea,    *rv);
    rv->resize_win(700,350);
    return rv;
  }
  
  DetailViewer::DetailViewer(QWidget* parent, const std::string& descriptor) : FViewer(parent), _descriptor(descriptor),
                                                                               _stackData(new QTableWidget()),
                                                                               _argData(new QTableWidget()),
                                                                               _model(_stackData, _argData,_items)
{
  _actionBar = new ActionBar(ActionBar::Close);
  _argData->setStyleSheet(_settings->value("traffic_grid_style").toString());
  _argData->verticalHeader()->hide();
  _argData->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  QWidget* holder = new QWidget();
  QVBoxLayout* vlayout = new QVBoxLayout();
  vlayout->setSpacing(0);
  vlayout->addWidget(_argData );
  vlayout->setContentsMargins(0,0,0,0);
  holder->setLayout(vlayout);
  holder->setStyleSheet(_settings->value("zero_border_style").toString());
   QWidget* holderStacks = new QWidget();
  vlayout = new QVBoxLayout();
  vlayout->setSpacing(0);
  _stackData->setStyleSheet(_settings->value("traffic_grid_style").toString());
  _stackData->horizontalHeader()->setStyleSheet("QTableWidget::item {background: #202020;}");
  _stackData->verticalHeader()->hide();
  _stackData->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  _stackData->insertColumn(0);
  _stackData->insertColumn(0);
  _stackData->setHorizontalHeaderItem(0, createItem("count"));
  _stackData->setHorizontalHeaderItem(1,createItem("stack"));
  _stackData->horizontalHeader()->setStretchLastSection(true);
  _stackData->setItemDelegateForColumn(1, new SignalDelegate(_stackData)); 

  KeyListener* tkeyListener = new KeyListener;
  _stackData->installEventFilter(tkeyListener);
  QObject::connect(tkeyListener, &KeyListener::signalkey, this, [&](){if(_stackData->currentColumn() == 1) add_signal(_stackData->currentItem()->text());});

  
  vlayout->addWidget(_stackData);
  holderStacks->setLayout(vlayout);
  holderStacks->setStyleSheet("QWidget { background-color: black;border: 0px;}");
  QTabWidget* tab = new QTabWidget();
  tab->addTab(holder, "Args / Instance data");
  tab->addTab(holderStacks, "Call Stacks");

  tab->setStyleSheet("QWidget {border: none; background:black;} "       \
                     "QTabBar::tab {height: 18px; background-color:#606060;color:#B4B6B6;font-size: 12px;border-top-left-radius:8px;min-width: 40ex; padding:3px;margin-left:2px;} " \
                       "QTabBar::tab:hover {color: white;border: 1px solid #B4B6B6;}"             \
                       "QTabBar::tab:selected {background: #173496;}" );
  tab->setDocumentMode(true);
  setup_dockwin(descriptor, tab, true);
}

  void DetailViewer::update(const std::string& descriptor, DetailHolder* holder)
  {
  if(_descriptor != descriptor) return;
  update_title(std::to_string(holder->_count) + "  " + _descriptor);

  _model.update_stack_view(holder->_stacks);
  _model.init_arg_instance_headers(holder->_argHeaders,holder->_instanceHeaders);

  for(auto& xitem : holder->_markers ) {
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
