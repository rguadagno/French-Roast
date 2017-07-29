// copyright (c) 2016-2017 Richard Guadagno
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

#include <QTWidgets/QLabel>
#include <QTWidgets/QHBoxLayout>
#include <QTWidgets/QListWidget>
#include <QTWidgets/QMessageBox>
#include <QTWidgets/QStatusBar>
#include <QTWidgets/QSizePolicy>
#include <QTWidgets/QPushButton>
#include <QTWidgets/QTableWidget>
#include <QTWidgets/QDockWidget>
#include <QTCore/QObject>
#include <QtGlobal>
#include <QAction>
#include <QFont>
#include <string>
#include "fr.h"
#include "FRMain.h"
#include "MonitorUtil.h"
#include <QDesktopWidget>
#include <QMenuBar>
#include <QToolBar>
//#include <algorithm>
//#include "CodeFont.h"
#include "FSignalViewer.h"
#include "FTimerViewer.h"
#include "FClassViewer.h"
#include "Editor.h"
#include "TrafficViewer.h"

using namespace frenchroast;

const std::string  FRMain::SignalWindow    = "signals";
const std::string  FRMain::EditHooksWindow = "editor";
const std::string  FRMain::TimerWindow     = "timers";
const std::string  FRMain::TrafficWindow   = "traffic";
const std::string  FRMain::ClassViewerWindow  = "classviewer";

std::unordered_map< std::string,  void (FRMain::*)()  > FRMain::_dockbuilders {{FRMain::SignalWindow,      &FRMain::view_signals},
                                                                               {FRMain::EditHooksWindow,   &FRMain::view_hooks_editor},
                                                                               {FRMain::TimerWindow,       &FRMain::view_timers},
                                                                               {FRMain::TrafficWindow,     &FRMain::view_traffic},
                                                                               {FRMain::ClassViewerWindow, &FRMain::view_classviewer}
      };


QSettings* FViewer::_settings = nullptr;

FRMain::FRMain( QSettings& settings, const std::string& path_to_hooks) : _settings(settings), _hooksfile(path_to_hooks)
{
  QDesktopWidget* dw = QApplication::desktop();

  int height = dw->availableGeometry(dw->primaryScreen()).height() * 0.2;
  int width  =  dw->availableGeometry(dw->primaryScreen()).width() * 0.7;

  FViewer::setSettings(&settings);
  
  QMenu* mptr = menuBar()->addMenu("&View");
  connect_dock_win(mptr, "Signals",         SignalWindow);
  connect_dock_win(mptr, "Timers",          TimerWindow);
  connect_dock_win(mptr, "Hooks Editor",    EditHooksWindow);
  connect_dock_win(mptr, "Traffic",         TrafficWindow);
  connect_dock_win(mptr, "Class Viewer",    ClassViewerWindow);
  
  resize(_settings.value("main:width", width).toInt(),
         _settings.value("main:height", height).toInt());

  move(_settings.value("main:xpos", 0).toInt(),
       _settings.value("main:ypos", 0).toInt());

  
  for(auto& x : _dockbuilders) {
    bring_up_dock_if_required(x.first);    
  }
  
  _statusMsg = new FRStatus{statusBar()};  
  statusBar()->addPermanentWidget(_statusMsg,10);
  _statusMsg->waiting_for_connection();
}



void FRMain::validate_hooks()
{
  QObject::connect(Editor::instance(this), &frenchroast::Editor::validated_hooks,  this,    &FRMain::validated_hooks);
  Editor::instance(this)->validate_hooks();
}


void FRMain::handshake()
{
  if(_watchTraffic) {
    start_traffic(_trafficRate);
  }
  _statusMsg->remote_ready();
}

void FRMain::remote_connected(const std::string& msg)
{
  _statusMsg->remote_connected(msg);
}

void FRMain::remote_disconnected(const std::string& msg)
{
  _statusMsg->remote_disconnected(msg);
}

void FRMain::connect_dock_win(QMenu* mptr, const std::string& actionname, const std::string& docname)
{
  QAction* act = mptr->addAction(QString::fromStdString(actionname));
  QObject::connect(act, &QAction::triggered, this, _dockbuilders[docname]);
}


void FunctionPoint::set_decorated_name(const QString& name)
{
  _name = name;
}

QString FunctionPoint::get_name() const
{
  return _name;
}

void FRMain::bring_up_dock_if_required(const std::string dockname)
{
  if(_settings.value(QString::fromStdString(dockname + ":up")).toBool() == true) {
    (this->*_dockbuilders[dockname])();
  }
}

void FRMain::view_traffic()
{
  QObject::connect(TrafficViewer::instance(this), &frenchroast::TrafficViewer::start_watching,  this, &FRMain::start_watching_traffic);
  QObject::connect(TrafficViewer::instance(this), &frenchroast::TrafficViewer::stop_watching,  this,  &FRMain::stop_watching_traffic);
}

void FRMain::view_classviewer()
{
  QObject::connect(FClassViewer::instance(this), &frenchroast::FClassViewer::start_watching, this,  &FRMain::start_watch_loading);
  QObject::connect(FClassViewer::instance(this), &frenchroast::FClassViewer::stop_watching, this,   &FRMain::stop_watch_loading);
  QObject::connect(FClassViewer::instance(this), &frenchroast::FClassViewer::add_signal, this,      &FRMain::add_hook);
  QObject::connect(FClassViewer::instance(this), &frenchroast::FClassViewer::closed, this,          &FRMain::stop_watch_loading);
}

void FRMain::start_watch_loading()
{
   start_loading();
}

void FRMain::stop_watch_loading()
{
   stop_loading();
}


void FRMain::view_signals()
{
  QObject::connect(frenchroast::FSignalViewer::instance(this),  &frenchroast::FSignalViewer::view_detail_request, this, &FRMain::show_detail);
}


void FRMain::view_timers()
{
  FTimerViewer::instance(this);
}

void FRMain::view_hooks_editor()
{
  QObject::connect(Editor::instance(this), &frenchroast::Editor::validated_hooks,  this,    &FRMain::validated_hooks);
}

void FRMain::add_hook(QString txt)
{
  Editor::instance(this)->add_hook(txt);
}

void FRMain::show_detail(const std::string& descriptor)
{
  DetailViewer* dv = DetailViewer::instance(this, descriptor);
  QObject::connect(this, &FRMain::update_detail_list, dv, &DetailViewer::update);
  QDockWidget* dock = *frenchroast::FSignalViewer::instance(this);
  dv->move(dock->x() + 50, dock->y() + 50 ); 
  update_detail_list(descriptor, _detailDescriptors[descriptor]);
}

void FRMain::update_class_viewer(const std::vector<frenchroast::monitor::ClassDetail>& details)
{
  FClassViewer::instance(this)->update(details);
}

void FRMain::start_watching_traffic(int rate)
{
  _watchTraffic = true;
  _trafficRate = rate;
  start_traffic(rate);
}

void FRMain::stop_watching_traffic()
{
  _watchTraffic = false;
  stop_traffic();
}


void FRMain::method_ranking(std::vector<frenchroast::monitor::MethodStats> ranks)
{
  TrafficViewer::instance(this)->update_ranking(ranks);
}

void FRMain::update_list(std::string ltype, std::string  descriptor, std::string tname, int count,
                         const std::vector<std::string> argHeaders,  const std::vector<std::string> instanceHeaders, 
                         const std::vector<frenchroast::monitor::MarkerField> markers, std::unordered_map<std::string, frenchroast::monitor::StackReport> stacks)
{
  tname = "[ " + tname + " ]";
  frenchroast::monitor::pad(descriptor, 50);
  frenchroast::monitor::pad(tname, 10);

  descriptor = tname + descriptor;
  _detailDescriptors[descriptor] = DetailHolder{count, argHeaders, instanceHeaders, markers, stacks};
  frenchroast::FSignalViewer::instance(this)->update_count(descriptor, count);
  if(!_exit) {
    update_detail_list(descriptor, _detailDescriptors[descriptor]);
  }
}

void FRMain::update_traffic(const std::vector<frenchroast::monitor::StackTrace>& stacks)
{
  if(!_exit) {
    TrafficViewer::instance(this)->update_traffic(stacks);
  }
}

void FRMain::update_timed_list(std::string  descriptor, std::string tname, long elapsed)
{
  std::string desc{descriptor};
  tname = "[ " + tname + " ]";
  frenchroast::monitor::pad(desc, 50);
  frenchroast::monitor::pad(tname, 10);
  desc = tname + desc;
  FTimerViewer::instance(this)->update_time(desc, elapsed);
}


void FRMain::handle_exit()
{
  _exit = true;
  _settings.setValue("main:width",  width());
  _settings.setValue("main:height",  height());
  _settings.setValue("main:xpos",   pos().x());
  _settings.setValue("main:ypos",   pos().y());

  FSignalViewer::capture();
  FTimerViewer::capture();
  Editor::capture();
  FClassViewer::capture();
  TrafficViewer::capture();
}


