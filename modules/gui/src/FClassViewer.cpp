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


#include "FClassViewer.h"
#include <QHeaderView>
#include <QVBoxLayout>
#include "QUtil.h"
#include "SignalDelegate.h"
#include "KeyListener.h"
#include "Util.h"
#include <iostream>


namespace frenchroast {
  
  FClassViewer::FClassViewer(QSettings& settings, QWidget* parent) : FViewer(settings, parent)
  {
    _data = new QTableWidget;
    _data->setStyleSheet(settings.value("traffic_grid_style").toString());
    _data->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    _data->verticalHeader()->hide();
    _data->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    _actionBar = new ActionBar(ActionBar::Close | ActionBar::StartStop);
    QObject::connect(_actionBar, &ActionBar::start_clicked, this, &FClassViewer::start_watching);
    QObject::connect(_actionBar, &ActionBar::stop_clicked, this,  &FClassViewer::stop_watching);
   
    QVBoxLayout* vlayout = new QVBoxLayout();
    vlayout->setSpacing(0);
    vlayout->addWidget(_data );
    vlayout->setContentsMargins(0,0,0,0);
    setLayout(vlayout);
    setStyleSheet(settings.value("zero_border_style").toString());
    
    _data->insertColumn(0);
    _data->setHorizontalHeaderItem(0, createItem("Class"));
    _data->setItemDelegateForColumn(0, new SignalDelegate(_data)); 

    setMinimumSize(700,350);
    KeyListener* keyListener = new KeyListener;
    _data->installEventFilter(keyListener);
    setup_dockwin("Loaded Classes", _data, false);
    QObject::connect(keyListener,   &KeyListener::enterkey,           this, [&](){ methods_for_class(_data->currentItem());});
    QObject::connect(keyListener  , &KeyListener::signalkey,          this, [&](){handle_add_signal(_data->currentRow(), _data->currentItem()->text());});
    QObject::connect(_data,         &QTableWidget::itemDoubleClicked, this, &FClassViewer::methods_for_class);
}

void FClassViewer::update(const std::vector<frenchroast::monitor::ClassDetail>& details)
{
  for(auto& citem : details) {
    int row = _data->rowCount();
    _data->insertRow(row);
    _data->setItem(row, 0, createItem(citem.name(), Qt::AlignLeft|Qt::AlignVCenter));
    _methods[citem.name()] = citem.methods();
  }
}



void FClassViewer::expand_methods(const std::string& name, int row)
{
  _ind[name] = 1;
  for(auto& mitem : _methods[name]) {
    _data->insertRow(++row);
    _data->setItem(row, 0, createItem("    " + mitem, Qt::AlignLeft|Qt::AlignVCenter));
  }
}

void FClassViewer::collapse_methods(const std::string& name, int row)
{
  _ind[name] = 0;
  ++row;
  for(auto& mitem : _methods[name]) {
    _data->removeRow(row);
  }
}

void FClassViewer::methods_for_class(QTableWidgetItem* item)
{
  std::string name = item->text().toStdString();
  if(_methods.count(name) == 0) return;

  if(_ind[name] == 0) {
    expand_methods(name, item->row());
  }
  else {
    collapse_methods(name, item->row());
  }
}

void FClassViewer::handle_add_signal(int row, QString text)
{
  std::string name = text.toStdString();
  frenchroast::remove_blanks(name);
  if(_methods.count(name) == 0) {
    while(--row >= 0) {
      if(_methods.count(_data->itemAt(0,row)->text().toStdString()) == 1) {
        add_signal(QString::fromStdString(_data->itemAt(0,row)->text().toStdString() + "::" + name + " "));
      }
    }

  }
  else {
    add_signal(QString::fromStdString(name + "::* "));
  }
}
  
}
