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

#include <iostream>
#include <QHeaderView>
#include <QVBoxLayout>
#include "QUtil.h"
#include "SignalDelegate.h"
#include "HeapDetailViewer.h"
#include "HeapReport.h"
#include "KeyListener.h"

namespace frenchroast {

  HeapDetailViewer* HeapDetailViewer::instance(QWidget* parent, const std::string& descriptor)
  {
    HeapDetailViewer* rv = new HeapDetailViewer(parent, descriptor);
    dynamic_cast<QMainWindow*>(parent)->addDockWidget(Qt::TopDockWidgetArea,    *rv);
    rv->resize_win(700,350);
    return rv;
  }
  
  HeapDetailViewer::HeapDetailViewer(QWidget* parent, const std::string& descriptor) : FViewer(parent),
                                                                                       _class_name(descriptor),
                                                                                       _data(new QTableWidget())
  {
    _actionBar = new ActionBar(ActionBar::Close);
    
    _data->setStyleSheet(_settings->value("traffic_grid_style").toString());
    _data->verticalHeader()->hide();
    _data->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    QVBoxLayout* vlayout = new QVBoxLayout();
    vlayout->setSpacing(0);
    vlayout->addWidget(_data );
    vlayout->setContentsMargins(0,0,0,0);
    setLayout(vlayout);
    setStyleSheet(_settings->value("zero_border_style").toString());

    _data->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);    
    
    _data->insertColumn(0);
    _data->setHorizontalHeaderItem(0, createItem("Lifetime"));
    _data->setItemDelegateForColumn(0, new SignalDelegate(_data)); 
    
    _data->insertColumn(1);
    _data->setHorizontalHeaderItem(1, createItem("Stack"));
    _data->setItemDelegateForColumn(1, new SignalDelegate(_data)); 

    _data->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    _data->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);

    setMinimumSize(1500,450);

    KeyListener* tkeyListener = new KeyListener;
    _data->installEventFilter(tkeyListener);
    QObject::connect(tkeyListener, &KeyListener::signalkey, this, [&](){
        if(_data->currentColumn() == 1 && _data->currentItem() != nullptr)
          add_signal(_data->currentItem()->text());
      }
      );
    setup_dockwin("Heap Monitor: " + _class_name, _data, false);
}

void HeapDetailViewer::update(const std::string& class_name, frenchroast::monitor::HeapReport* rpt)
  {
    if(_class_name != class_name) return;

    for(auto& sr : rpt->stacks()) {
      if(_lifetime_widgets.find(sr.second.key()) == _lifetime_widgets.end()) {
        _lifetime_widgets[sr.second.key()] = createItem(sr.second.count());
        int row = _data->rowCount();
        _data->insertRow(row);
        _data->setItem(row,0, _lifetime_widgets[sr.second.key()]);
        for(auto& desc : sr.second.descriptors()) {
          _data->setItem(row,1, createItem(desc.descriptor()));
          _data->insertRow(++row);
        }
      }
      else {
        _lifetime_widgets[sr.second.key()]->setText( QString::fromStdString(std::to_string(sr.second.count())));
      }
    }
  }
}
