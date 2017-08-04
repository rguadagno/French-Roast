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
#include <QTabWidget>
#include <QVBoxLayout>

namespace frenchroast {

AboutHelpViewer::AboutHelpViewer(QWidget* parent) : FViewer(parent)
  {
    _statusMsg = new FRStatus{};  
    _statusMsg->waiting_for_connection();

    QWidget* holder = new QWidget();
    QVBoxLayout* vlayout = new QVBoxLayout();
    vlayout->setSpacing(0);
    vlayout->addWidget(_statusMsg );
    vlayout->setContentsMargins(0,0,0,0);
    holder->setLayout(vlayout);
    holder->setStyleSheet(_settings->value("zero_border_style").toString());

    
    QTabWidget* tab = new QTabWidget();
    tab->addTab(holder, "Targets");
    //    tab->addTab(holderStacks, "About");
    //tab->addTab(holderStacks, "Help");

    tab->setStyleSheet("QWidget {border: none; background:black;} "     \
                     "QTabBar::tab {height: 18px; background-color:#606060;color:#B4B6B6;font-size: 12px;border-top-left-radius:8px;min-width: 40ex; padding:3px;margin-left:2px;} " \
                       "QTabBar::tab:hover {color: white;border: 1px solid #B4B6B6;}"             \
                       "QTabBar::tab:selected {background: #173496;}" );
    tab->setDocumentMode(true);

    setup_dockwin("French-Roast", tab, false);
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
