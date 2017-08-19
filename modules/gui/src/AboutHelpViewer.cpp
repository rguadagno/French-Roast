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

#include "AboutHelpViewer.h"
#include <QVBoxLayout>

namespace frenchroast {

AboutHelpViewer::AboutHelpViewer(QWidget* parent) : FViewer(parent)
  {
    QCheckBox* autoConnect = new QCheckBox("auto connect");
    autoConnect->setStyleSheet("QCheckBox{border: none; background:#404040;color:#C0C0C0;font-size:14px;}");
    autoConnect->setCheckState(Qt::Checked);
    _actionBar->add(autoConnect);
    _statusMsg = new FRStatus{autoConnect};
    QObject::connect(_statusMsg, &FRStatus::turn_on_profiler, this, &AboutHelpViewer::turn_on_profiler);
    QObject::connect(_statusMsg, &FRStatus::turn_off_profiler, this, &AboutHelpViewer::turn_off_profiler);

    QWidget* holder = new QWidget();
    QVBoxLayout* vlayout = new QVBoxLayout();
    vlayout->setSpacing(0);
    vlayout->addWidget(_statusMsg );
    vlayout->setContentsMargins(0,0,0,0);
    holder->setLayout(vlayout);
    holder->setStyleSheet(_settings->value("zero_border_style").toString());
    setup_dockwin("Targets", holder, false);
  }



  AboutHelpViewer* AboutHelpViewer::_instance{nullptr};
  const std::string AboutHelpViewer::FName{"about"};
  

  AboutHelpViewer::~AboutHelpViewer()
  {
    _instance = nullptr;
  }

  AboutHelpViewer* AboutHelpViewer::instance(QWidget* parent)
  {
    if(_instance != nullptr) return _instance;
    _instance = new AboutHelpViewer(parent);
    restore_win(FName, _settings, _instance, dynamic_cast<QMainWindow*>(parent));
    return _instance;
  }



  void AboutHelpViewer::capture()
  {
     capture_win(FName, _settings, _instance != nullptr ? _instance : nullptr);
  }

  bool AboutHelpViewer::restore_is_required()
  {
    return restore_required(FName, _settings);
  }


  void AboutHelpViewer::remote_connected(const std::string& host, const std::string& pid)
  {
    _statusMsg->remote_connected(host,pid);
  }
  
  void AboutHelpViewer::remote_disconnected(const std::string& host, const std::string& pid)
  {
    _statusMsg->remote_disconnected(host,pid);
  }
  
  void AboutHelpViewer::remote_ready(const std::string& host, const std::string& pid)
  {
    _statusMsg->remote_ready(host, pid);
  }


  
  
}
