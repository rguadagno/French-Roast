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
#include <QMainWindow>

namespace frenchroast {

  FTimerViewer::FTimerViewer(QWidget* parent) : FViewer(parent)
  {
    _data = new QListWidget{};
    _data->setStyleSheet(_settings->value("list_style").toString());
    _actionBar = new ActionBar{ActionBar::Close};
    setup_dockwin("Timers", _data, false);
  }


  void FTimerViewer::update_time(const std::string& descriptor, long elapsed)
  {
    if (_descriptors.count(descriptor) == 0 ) {
      _descriptors[descriptor] = new FListItem(descriptor, frenchroast::monitor::format_millis(elapsed));
      _data->addItem(_descriptors[descriptor]);
    }
    else {
      _descriptors[descriptor]->setText(QString::fromStdString(frenchroast::monitor::format_millis(elapsed)) + "   "  + QString::fromStdString(descriptor) );
    }
  }
  
  FTimerViewer* FTimerViewer::_instance{nullptr};
  
  FTimerViewer::~FTimerViewer()
  {
    _instance = nullptr;
  }


  FTimerViewer* FTimerViewer::instance(QWidget* parent)
  {
    if(_instance != nullptr) return _instance;
    _instance = new FTimerViewer(parent);
    restore_win("timers", _settings, _instance->_dock, dynamic_cast<QMainWindow*>(parent));
    return _instance;
  }

  void FTimerViewer::capture()
  {
     capture_win("timers", _settings, _instance != nullptr ? _instance->_dock : nullptr);
  }


}
