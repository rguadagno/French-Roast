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

namespace frenchroast {

FSignalViewer::FSignalViewer(QWidget* parent) : FViewer(parent)
  {
    _data = new QListWidget{};
    _data->setStyleSheet(_settings->value("list_style").toString());
    _actionBar = new ActionBar{ActionBar::Close};
    setup_dockwin("Signals", _data, false);
    QObject::connect(_data,  &QListWidget::itemDoubleClicked, this, [&](QListWidgetItem* item){view_detail_request(dynamic_cast<FListItem*>(item)->gettext());});
  }


  void FSignalViewer::update_count(const std::string& descriptor, int count)
  {
    if (_descriptors.count(descriptor) == 0 ) {
      _descriptors[descriptor] = new FListItem(descriptor, frenchroast::monitor::ntoa(count,5,' '));
      _data->addItem(_descriptors[descriptor]);
    }
    else {
      _descriptors[descriptor]->setText(QString::fromStdString(frenchroast::monitor::ntoa(count,5, ' ')) + "   " + QString::fromStdString(descriptor) );
    }
  }

  FSignalViewer* FSignalViewer::_instance{nullptr};
  FSignalViewer::~FSignalViewer()
  {
    _instance = nullptr;
  }

  FSignalViewer* FSignalViewer::instance(QWidget* parent)
  {
    if(_instance != nullptr) return _instance;
    _instance = new FSignalViewer(parent);
    restore_win("signals", _settings, _instance, dynamic_cast<QMainWindow*>(parent));
    return _instance;
  }



  void FSignalViewer::capture()
  {
     capture_win("signals", _settings, _instance != nullptr ? _instance : nullptr);
  }

}
