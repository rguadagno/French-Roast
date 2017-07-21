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


#include "DetailViewer.h"
#include <QHeaderView>
#include <QVBoxLayout>
#include "QUtil.h"
#include "SignalDelegate.h"

void DetailViewer::setTitleWidget(QListWidgetItem* wptr)
{
  _titleWidget = wptr;
  _title = wptr->text();
}

DetailViewer::DetailViewer(const std::string& descriptor, QSettings& settings) : _descriptor(descriptor)
{
  _argData = new QTableWidget;
  _argData->setStyleSheet(settings.value("traffic_grid_style").toString());
  _argData->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
  _argData->verticalHeader()->hide();
  _argData->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  QWidget* holder = new QWidget();
  QVBoxLayout* vlayout = new QVBoxLayout();
  vlayout->setSpacing(0);
  vlayout->addWidget(_argData );
  vlayout->setContentsMargins(0,0,0,0);
  holder->setLayout(vlayout);
  holder->setStyleSheet(settings.value("zero_border_style").toString());
   QWidget* holderStacks = new QWidget();
  vlayout = new QVBoxLayout();
  vlayout->setSpacing(0);
  _stackData = new QTableWidget();
  _stackData->setStyleSheet(settings.value("traffic_grid_style").toString());
  _stackData->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
  _stackData->horizontalHeader()->setStyleSheet("QTableWidget::item {background: #202020;}");
  _stackData->verticalHeader()->hide();
  _stackData->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  _stackData->insertColumn(0);
  _stackData->insertColumn(0);
  _stackData->setHorizontalHeaderItem(0, createItem("count"));
  _stackData->setHorizontalHeaderItem(1,createItem("stack"));
  _stackData->setItemDelegateForColumn(1, new SignalDelegate(_stackData)); 
  
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
  QVBoxLayout* vlayout2 = new QVBoxLayout;
  vlayout2->setContentsMargins(0,0,0,0);
  vlayout2->addWidget(tab);
  setLayout(vlayout2);
  setMinimumSize(700,350);
   setStyleSheet("QWidget {border: 1px solid #404040;background:black;}");
}

void DetailViewer::update(const std::string& descriptor, const DetailHolder& holder)
{
  if(_descriptor != descriptor) return;

  _titleWidget->setText(QString::number(holder._count) + "  " + _title);
  for(auto& x : holder._stacks) {
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
    }
  }
  
  if(_argData->rowCount() == 0) {

    _argData->insertColumn(0);
    _argData->insertColumn(0);
    _argData->setHorizontalHeaderItem(0, createItem("invoked"));
    _argData->setHorizontalHeaderItem(1,createItem("("));
    
    int colidx = 2;
    for(auto& x : holder._argHeaders) {
      _argData->insertColumn(colidx);
      _argData->setHorizontalHeaderItem(colidx++,createItem(x));
    }

    _argData->insertColumn(colidx);
    _argData->setHorizontalHeaderItem(colidx++,createItem(")"));

    for(auto& x : holder._instanceHeaders) {
      _argData->insertColumn(colidx);
      _argData->setHorizontalHeaderItem(colidx++,createItem(x));
    }
  }


  for(auto& item : holder._markers) {
  if(_detailItems.count(item._descriptor) == 0 ) {
    if(item._count > 1) {
      int currRow = _argData->rowCount();
      _argData->insertRow(currRow);
      _detailItems[item._descriptor] = currRow;
      _argData->setItem(currRow, 0, createItem(item._count));
      _argData->setItem(currRow, 1, createItem("("));
      
      int colidx = 2;
      int idx=1;
      int totalargs = item._arg_items.size();
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
      _detailItems[item._descriptor] = -1;
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
    if(_detailItems[item._descriptor] == -1  ) {
      if(item._count == 1) continue;
      QTableWidgetItem* titem = _argData->item(_detailItems["*"], 0);
      int total = titem->text().toInt() - 1;
      titem->setText( QString::number(total));
      int currRow = _argData->rowCount();
      _argData->insertRow(currRow);
      _detailItems[item._descriptor] = currRow;
      _argData->setItem(currRow, 0, createItem(item._count));
      _argData->setItem(currRow, 1, createItem("("));
      int colidx = 2;
      int idx=1;
      int totalargs = item._arg_items.size();
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
      _argData->item(_detailItems[item._descriptor],0)->setText(  QString::fromStdString(std::to_string(item._count)) );
    }
  }
  }
}
