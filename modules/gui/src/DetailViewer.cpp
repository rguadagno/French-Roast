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
    _model.update_args_markers(holder->_markers);
  }

}
