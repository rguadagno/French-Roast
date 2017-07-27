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
#include <QHeaderView>
#include <QAction>
#include <QFont>
#include <string>
#include "fr.h"
#include "FRMain.h"
#include "MonitorUtil.h"
#include "StackRow.h"
#include <QDesktopWidget>
#include <QMenuBar>
#include <QToolBar>
#include <algorithm>
#include "CodeFont.h"
#include "SignalDelegate.h"
#include "FSignalViewer.h"

using namespace frenchroast;

const std::string  FRMain::SignalWindow    = "signals";
const std::string  FRMain::EditHooksWindow = "editor";
const std::string  FRMain::TimerWindow     = "timers";
const std::string  FRMain::RankingWindow   = "rankings";
const std::string  FRMain::TrafficWindow   = "traffic";
const std::string  FRMain::ClassViewerWindow  = "classviewer";

std::unordered_map< std::string,  void (FRMain::*)()  > FRMain::_dockbuilders {{FRMain::SignalWindow,      &FRMain::view_signals},
                                                                               {FRMain::EditHooksWindow,   &FRMain::view_hooks_editor},
                                                                               {FRMain::TimerWindow,       &FRMain::view_timers},
                                                                               {FRMain::RankingWindow,     &FRMain::view_ranking},
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
  connect_dock_win(mptr, "Method Rankings", RankingWindow);
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

QFont StackRow::_font = CodeFont();

void FRMain::validate_hooks()
{
  QObject::connect(Editor::instance(this), &frenchroast::Editor::validated_hooks,  this,    &FRMain::validated_hooks);
  Editor::instance(this)->validate_hooks();
}


void FRMain::handshake()
{
  if(_docks.count(TrafficWindow) == 1 && _buttonStartTraffic->text() == "Stop") {
    start_traffic(atoi(_rate->text().toStdString().c_str()));
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

QDockWidget* FRMain::setup_dock_window(const std::string& title, QWidget* wptr, ActionBar* actionptr, bool codeMode,  QListWidgetItem** titleItem)
{
  QDockWidget* holder = new QDockWidget(QString::fromStdString(title), this);
  holder->setStyleSheet(_settings.value("dock_widget_style").toString());
  QListWidget* sigLabel = new QListWidget{};
  QWidget*     titlebar = new QWidget();
  QGridLayout* layout = new QGridLayout();

  
  QListWidgetItem* item = new QListWidgetItem{QString::fromStdString(title)};
  item->setSizeHint(QSize(30,24));
  sigLabel->setEnabled(false);
  sigLabel->setFixedHeight(22);
  sigLabel->addItem(item);
  sigLabel->setStyleSheet(_settings.value("dock_title_style2").toString());
  if(codeMode) {
    sigLabel->setItemDelegate(new SignalDelegate(sigLabel));
    *titleItem = item;
  }

  layout->addWidget(sigLabel,1,1);
  layout->setContentsMargins(1,1,1,1);
  layout->addWidget(actionptr,1, 5);
  titlebar->setLayout(layout);
  titlebar->setStyleSheet(_settings.value("dock_win_header_style").toString());

  holder->setTitleBarWidget(titlebar);
  holder->setAttribute(Qt::WA_DeleteOnClose);
  holder->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
  holder->setWidget(wptr);
  
  return holder;
}

void FRMain::connect_dock_win(QMenu* mptr, const std::string& actionname, const std::string& docname)
{
  QAction* act = mptr->addAction(QString::fromStdString(actionname));
  QObject::connect(act, &QAction::triggered, this, _dockbuilders[docname]);
}

void FRMain::restore_dock_win(const std::string& dockname)
{
  _docks[dockname]->setFloating(true);
  _settings.setValue(QString::fromStdString(dockname + ":up"), true);
  _docks[dockname]->resize(_settings.value(QString::fromStdString(dockname + ":width"), width()).toInt(),
                           _settings.value(QString::fromStdString(dockname + ":height"), 200).toInt()
                           );

  _docks[dockname]->move(_settings.value(QString::fromStdString(dockname + ":xpos"), width() /2).toInt(),
                         _settings.value(QString::fromStdString(dockname + ":ypos"), height() /2).toInt());
  addDockWidget(Qt::TopDockWidgetArea,    _docks[dockname]);
}



QWidget* FRMain::build_traffic_viewer(QTableWidget* grid, QPushButton* bstart, QLineEdit* rate)
{
  QWidget* buttonHolder = new QWidget;
    QGridLayout* hlayout = new QGridLayout();

  QLabel* desc = new QLabel{QString::fromStdString("sampling rate (millisec):")};
  desc->setAlignment(Qt::AlignRight | Qt::AlignCenter);
  desc->setStyleSheet(_settings.value("descriptive_text_style").toString());
  buttonHolder->setLayout(hlayout);
  rate->setText("100");
  
  hlayout->addWidget(desc,0,0, Qt::AlignRight);
  hlayout->addWidget(rate,0,1, Qt::AlignLeft);
  hlayout->addWidget(bstart,0,2);
  hlayout->setColumnStretch(3,10);
  

  buttonHolder->setStyleSheet(_settings.value("button_holder_style").toString());

  rate->setStyleSheet(_settings.value("data_entry_style").toString());
  rate->setFixedWidth(50);
  rate->setInputMask("0000");
  rate->setAlignment(Qt::AlignRight);
  bstart->setStyleSheet("QPushButton {padding:1px;border: 1px solid #404040;border-top-left-radius:2px;border-top-right-radius:2px;border-bottom-right-radius:2px; border-bottom-left-radius:2px;font-size: 16px;color:black;background-color: #08b432;font-family:\"Arial\";} QPushButton::hover{color:white;border-color:white;}");
  
  bstart->setFixedWidth(70);
  QWidget* holder = new QWidget();
  QVBoxLayout* vlayout = new QVBoxLayout();
  vlayout->setSpacing(0);
  holder->setLayout(vlayout);
  grid->setStyleSheet(_settings.value("traffic_grid_style").toString());
  grid->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
  grid->horizontalHeader()->hide();
  grid->verticalHeader()->hide();
  grid->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  vlayout->addWidget(grid);
  vlayout->addWidget(buttonHolder);
  vlayout->setContentsMargins(1,0,1,0);
  holder->setStyleSheet(_settings.value("zero_border_style").toString());
  return holder;
}



QWidget* setup_central(QWidget* lists)
{
  QWidget* holder = new QWidget;
  holder->setStyleSheet("QWidget {background-color: black;}");
  QVBoxLayout* vlayout = new QVBoxLayout();
  vlayout->setSpacing(0);
  holder->setLayout(vlayout);
  vlayout->addWidget(lists);
  return holder;
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

  if(_docks.count(TrafficWindow) == 1) return;

  _traffic            = new QTableWidget;
  _buttonStartTraffic = new QPushButton{"Start"};
  _rate               = new QLineEdit;
  _trafficEnterKeyListener = new KeyListener;
  _traffic->installEventFilter(_trafficEnterKeyListener);

  view_dockwin("Traffic", TrafficWindow, build_traffic_viewer(_traffic, _buttonStartTraffic, _rate));
  
  _traffic->insertColumn(0);
  QObject::connect(_buttonStartTraffic,      &QPushButton::clicked,         this, &FRMain::update_traffic_rate);
  QObject::connect(_trafficEnterKeyListener, &KeyListener::enterkey,   this, [&](){ add_hook(_traffic->currentItem()->text());});
  QObject::connect(_traffic,                 &QTableWidget::destroyed,      this, [&](){if(_exit) return;_traffic_rows.clear(); _traffic_keys.clear();});
}

void FRMain::view_classviewer()
{
 if(_docks.count(ClassViewerWindow) == 1) return;
  _classViewer = new frenchroast::FClassViewer(_settings, this);
  _docks[ClassViewerWindow] = *_classViewer;
  QObject::connect(_classViewer, &frenchroast::FClassViewer::closed, this, [&](){_docks.erase(ClassViewerWindow);});
  QObject::connect(_classViewer, &frenchroast::FClassViewer::start_watching, this,                &FRMain::start_watch_loading);
  QObject::connect(_classViewer, &frenchroast::FClassViewer::stop_watching, this,                 &FRMain::stop_watch_loading);
  QObject::connect(_classViewer, &frenchroast::FClassViewer::add_signal, this,         &FRMain::add_hook);
  restore_dock_win(ClassViewerWindow);
}

void FRMain::start_watch_loading()
{
   start_loading();
}

void FRMain::stop_watch_loading()
{
   stop_loading();
}

void FRMain::view_ranking()
{
  if(_docks.count(RankingWindow) == 1) return;
  view_dockwin("Ranking", RankingWindow, (_rankings = new MethodRanking()));

  KeyListener* rankListener = new KeyListener;
  _rankings->installEventFilter(rankListener);
  
  QObject::connect(rankListener, &KeyListener::enterkey,           this, [&](){ add_hook(_rankings->getEnterMethod());});
  QObject::connect(_rankings,    &QListWidget::itemDoubleClicked,  this, [&](){ add_hook(_rankings->getEnterMethod());});
}

void FRMain::view_signals()
{
  QObject::connect(frenchroast::FSignalViewer::instance(this),  &frenchroast::FSignalViewer::view_detail_request, this, &FRMain::show_detail);
}


void FRMain::view_timers()
{
  FTimerViewer::instance(this);
}


void FRMain::view_dockwin(const std::string& title, const std::string& dockname, QWidget* wptr)
{
  ActionBar* abar = new ActionBar(ActionBar::Close);
  wptr->setStyleSheet(_settings.value("list_style").toString());
  _docks[dockname] = setup_dock_window(title, wptr, abar);
  QObject::connect(abar, &ActionBar::close_clicked, _docks[dockname   ], &QDockWidget::close);
  QObject::connect(abar, &ActionBar::close_clicked, this,                [&](){_docks.erase(dockname);});
  restore_dock_win(dockname);
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
  if(_viewingDetail.count(descriptor) == 1) return;
  _viewingDetail[descriptor] = new DetailViewer{descriptor, _settings};
  ActionBar* abar = new ActionBar(ActionBar::Close);
  _viewingDetail[descriptor]->setStyleSheet(_settings.value("tab_style").toString());
  QListWidgetItem* titleWidget;
  QDockWidget* dockwin = setup_dock_window(descriptor, _viewingDetail[descriptor], abar, true, &titleWidget);
  _viewingDetail[descriptor]->setTitleWidget(titleWidget);
  QObject::connect(abar, &ActionBar::close_clicked, dockwin, &QDockWidget::close);
  QObject::connect(abar, &ActionBar::close_clicked, this, [=](){ _viewingDetail.erase(descriptor); });
  QObject::connect(this, &FRMain::update_detail_list, _viewingDetail[descriptor], &DetailViewer::update);
  dockwin->setFloating(true);
  //@@  dockwin->move(_docks[SignalWindow]->x() + 50, _docks[SignalWindow]->y() + 50 );
  QDockWidget* dock = *frenchroast::FSignalViewer::instance(this);
  dockwin->move(dock->x() + 50, dock->y() + 50 ); 
  addDockWidget(Qt::TopDockWidgetArea, dockwin);
  update_detail_list(descriptor, _detailDescriptors[descriptor]);
}

void FRMain::update_class_viewer(const std::vector<frenchroast::monitor::ClassDetail>& details)
{
  if(_docks.count(ClassViewerWindow) == 0) return;
  _classViewer->update(details);
}

void FRMain::update_traffic_rate()
{
  if(_buttonStartTraffic->text() == "Start") {
     start_traffic(atoi(_rate->text().toStdString().c_str()));
    _buttonStartTraffic->setText("Stop");
    _buttonStartTraffic->setStyleSheet(  _settings.value("traffic_stop_style").toString());
  }
  else {
    _buttonStartTraffic->setText("Start");
    _buttonStartTraffic->setStyleSheet(  _settings.value("traffic_start_style").toString());
    stop_traffic();
 
  }
}

void FRMain::method_ranking(std::vector<frenchroast::monitor::MethodStats> ranks)
{
  _rankings->update(ranks);
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
  for(auto& x : stacks) {
    int currRow = _traffic->rowCount();
    if(_traffic_rows.count(x.thread_name()) == 0) {
      _traffic->insertRow(currRow);
      StackRow* sr = new StackRow{x.thread_name(), currRow, _traffic, _traffic_keys};
      _traffic_rows[x.thread_name()] = sr;
    }
    _traffic_rows[x.thread_name()]->add(x);
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

  for(auto& x : _dockbuilders) {
     capture_dock(x.first);
  }

  FSignalViewer::capture();
  FTimerViewer::capture();
  Editor::capture();
}

void FRMain::capture_dock( const std::string& dockname)
{
  if(_docks.count(dockname) == 1 && _docks[dockname] != nullptr) {
    _settings.setValue(QString::fromStdString(dockname + ":up"),    true);
    _settings.setValue(QString::fromStdString(dockname + ":width"),  _docks[dockname]->width());
    _settings.setValue(QString::fromStdString(dockname + ":height"), _docks[dockname]->height());
    _settings.setValue(QString::fromStdString(dockname + ":xpos"),   _docks[dockname]->pos().x());
    _settings.setValue(QString::fromStdString(dockname + ":ypos"),   _docks[dockname]->pos().y());
  }
  else {
    _settings.setValue(QString::fromStdString(dockname + ":up"),    false);
  }
}
