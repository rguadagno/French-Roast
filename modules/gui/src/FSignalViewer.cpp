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

#include "FSignalViewer.h"
#include "MonitorUtil.h"
#include <QMainWindow>
#include <QHeaderView>
#include <QApplication>
#include "SignalDelegate.h"
#include "QUtil.h"

namespace frenchroast {

  FSignalViewer::FSignalViewer(QWidget* parent) : FViewer(parent)
  {

    _data = new QTableWidget();
    _data->setStyleSheet(_settings->value("traffic_grid_style").toString());
    _data->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    _data->horizontalHeader()->hide();
    _data->verticalHeader()->hide();
    _data->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    _data->insertColumn(0);
    _data->insertColumn(0);
    _data->setItemDelegateForColumn(1, new SignalDelegate(_data));
    setup_dockwin("Signals", _data, false);
    QObject::connect(_data,  &QTableWidget::itemDoubleClicked, this, [&](QTableWidgetItem* item){if(_data->currentColumn() == 1) view_detail_request(item->text().toStdString());});

  }


  void FSignalViewer::update_count(const std::string& descriptor, int count )
  {
    if (_descriptors.count(descriptor) == 0 ) {
      _descriptors[descriptor] = createItem(count);
      addRow(_data, _descriptors[descriptor], createItem(descriptor));
    }
    else {
      _descriptors[descriptor]->setText(QString::fromStdString(frenchroast::monitor::ntoa(count,5, ' ')));
    }
  }

  FSignalViewer* FSignalViewer::_instance{nullptr};
  const std::string FSignalViewer::FName{"signals"};
  
  FSignalViewer::~FSignalViewer()
  {
    _instance = nullptr;
  }

  FSignalViewer* FSignalViewer::instance(QWidget* parent)
  {
    if(_instance != nullptr) return _instance;
    _instance = new FSignalViewer(parent);
    restore_win(FName, _settings, _instance, dynamic_cast<QMainWindow*>(parent));
    return _instance;
  }



  void FSignalViewer::capture()
  {
     capture_win(FName, _settings, _instance != nullptr ? _instance : nullptr);
  }

  bool FSignalViewer::restore_is_required()
  {
    return restore_required(FName, _settings);
  }

}
