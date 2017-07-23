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


#include "ClassViewer.h"
#include <QHeaderView>
#include <QVBoxLayout>
#include "QUtil.h"
#include "SignalDelegate.h"

ClassViewer::ClassViewer(QSettings& settings)
{
  _data = new QTableWidget;
  _data->setStyleSheet(settings.value("traffic_grid_style").toString());
  _data->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
  _data->verticalHeader()->hide();
  _data->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  
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
  QObject::connect(_data, &QTableWidget::itemDoubleClicked, this, &ClassViewer::methods_for_class);
}

void ClassViewer::update(const std::vector<frenchroast::monitor::ClassDetail>& details)
{
  for(auto& citem : details) {
    int row = _data->rowCount();
    _data->insertRow(row);
    _data->setItem(row, 0, createItem(citem.name(), Qt::AlignLeft|Qt::AlignVCenter));
    _methods[citem.name()] = citem.methods();
  }
}



void ClassViewer::expand_methods(const std::string& name, int row)
{
  _ind[name] = 1;
  for(auto& mitem : _methods[name]) {
    _data->insertRow(++row);
    _data->setItem(row, 0, createItem("    " + mitem, Qt::AlignLeft|Qt::AlignVCenter));
  }
}

void ClassViewer::collapse_methods(const std::string& name, int row)
{
  _ind[name] = 0;
  ++row;
  for(auto& mitem : _methods[name]) {
    _data->removeRow(row);
  }
}

void ClassViewer::methods_for_class(QTableWidgetItem* item)
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
