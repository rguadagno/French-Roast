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

#include "FTimerViewer.h"
#include "MonitorUtil.h"
#include "QUtil.h"
#include "SignalDelegate.h"
#include <QMainWindow>
#include <QHeaderView>

namespace frenchroast {

  FTimerViewer::FTimerViewer(QWidget* parent) : FViewer(parent)
  {
    _data = new QTableWidget{};
    _data->setStyleSheet(_settings->value("traffic_grid_style").toString());
    _data->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    _data->horizontalHeader()->hide();
    _data->verticalHeader()->hide();
    _data->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    _data->insertColumn(0);
    _data->insertColumn(0);
    _data->setItemDelegateForColumn(1, new SignalDelegate(_data));

    setup_dockwin("Timers", _data, false);
  }


  void FTimerViewer::update_time(const std::string& descriptor, long elapsed)
  {
    if (_descriptors.count(descriptor) == 0 ) {
      _descriptors[descriptor] = createItem(frenchroast::monitor::format_millis(elapsed));
      addRow(_data, _descriptors[descriptor], createItem(descriptor));
    }
    else {
      _descriptors[descriptor]->setText(QString::fromStdString(frenchroast::monitor::format_millis(elapsed)));
    }
  }
  
  FTimerViewer* FTimerViewer::_instance{nullptr};
  const std::string FTimerViewer::FName{"timers"};
  
  FTimerViewer::~FTimerViewer()
  {
    _instance = nullptr;
  }


  FTimerViewer* FTimerViewer::instance(QWidget* parent)
  {
    if(_instance != nullptr) return _instance;
    _instance = new FTimerViewer(parent);
    restore_win(FName, _settings, _instance, dynamic_cast<QMainWindow*>(parent));
    return _instance;
  }

  void FTimerViewer::capture()
  {
     capture_win(FName, _settings, _instance != nullptr ? _instance : nullptr);
  }

  bool FTimerViewer::restore_is_required()
  {
    return restore_required(FName, _settings);
  }

  void FTimerViewer::reset()
  {
    if(_instance == nullptr) return;
    _instance->reset_all();
  }

  void FTimerViewer::reset_all()
  {
    clearTable(_data);
    _descriptors.clear();
  }
  
}
