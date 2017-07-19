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
#include <QTabWidget>
#include "CodeFont.h"



const std::string  FRMain::SignalWindow    = "signals";
const std::string  FRMain::EditHooksWindow = "edit-hooks";
const std::string  FRMain::TimerWindow     = "timers";
const std::string  FRMain::RankingWindow   = "rankings";
const std::string  FRMain::TrafficWindow   = "traffic";

std::unordered_map< std::string,  void (FRMain::*)()  > FRMain::_dockbuilders {{FRMain::SignalWindow,    &FRMain::view_signals},
                                                                               {FRMain::EditHooksWindow, &FRMain::view_hooks_editor},
                                                                               {FRMain::TimerWindow,     &FRMain::view_timers},
                                                                               {FRMain::RankingWindow,   &FRMain::view_ranking},
                                                                               {FRMain::TrafficWindow,   &FRMain::view_traffic}
      };


FRMain::FRMain( QSettings& settings, const std::string& path_to_hooks) : _settings(settings), _hooksfile(path_to_hooks)
{
  QDesktopWidget* dw = QApplication::desktop();
  int height = dw->availableGeometry(dw->primaryScreen()).height() * 0.2;
  int width  = dw->availableGeometry(dw->primaryScreen()).width() * 0.7;

  QMenu* mptr = menuBar()->addMenu("&View");
  connect_dock_win(mptr, "Signals",         SignalWindow);
  connect_dock_win(mptr, "Timers",          TimerWindow);
  connect_dock_win(mptr, "Hooks Editor",    EditHooksWindow);
  connect_dock_win(mptr, "Method Rankings", RankingWindow);
  connect_dock_win(mptr, "Traffic",         TrafficWindow);
  
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

class SignalItem : public QListWidgetItem {
  std::string  _text;
  static QFont _font;
public:
  SignalItem(const std::string& text, const std::string& total) : _text(text), QListWidgetItem( QString::fromStdString(text) + "  " + QString::fromStdString(total))
  {
    setFont(_font);
  }

  const std::string& gettext() const
  {
    return _text;
  }

};


QFont SignalItem::_font = CodeFont();
QFont StackRow::_font = CodeFont();

void FRMain::validate_hooks()
{
  if(_editor == nullptr) return;
  _editor->validate_hooks();
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

QDockWidget* FRMain::setup_dock_window(const std::string& title, QWidget* wptr, ActionBar* actionptr, const std::string& wstyle, QDockWidget::DockWidgetFeatures features)
{
  QDockWidget* holder = new QDockWidget(QString::fromStdString(title), this);
  wptr->setStyleSheet(_settings.value(QString::fromStdString(wstyle)).toString());
  holder->setStyleSheet(_settings.value("dock_widget_style").toString());
  QLabel* sigLabel = new QLabel(QString::fromStdString(title));
  QWidget* titlebar = new QWidget();
  QGridLayout* layout = new QGridLayout();
  
  layout->addWidget(sigLabel,1,1);
  layout->setColumnStretch(1,10);
  layout->setContentsMargins(1,1,1,1);
  layout->addWidget(actionptr,1, 5);
  titlebar->setLayout(layout);
  titlebar->setStyleSheet(_settings.value("dock_win_header_style").toString());
  sigLabel->setStyleSheet(_settings.value("dock_title_style").toString());
  holder->setTitleBarWidget(titlebar);
  holder->setAttribute(Qt::WA_DeleteOnClose);
  holder->setFeatures(features);
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
  _trafficEnterKeyListener = new EnterKeyListener;
  _traffic->installEventFilter(_trafficEnterKeyListener);

  view_dockwin("Traffic", TrafficWindow, build_traffic_viewer(_traffic, _buttonStartTraffic, _rate));
  
  _traffic->insertColumn(0);
  QObject::connect(_buttonStartTraffic,      &QPushButton::clicked,         this, &FRMain::update_traffic_rate);
  QObject::connect(_trafficEnterKeyListener, &EnterKeyListener::enterkey,   this, [&](){ add_hook(_traffic->currentItem()->text());});
  QObject::connect(_traffic,                 &QTableWidget::destroyed,      this, [&](){if(_exit) return;_traffic_rows.clear(); _traffic_keys.clear();});
}

void FRMain::view_ranking()
{
  if(_docks.count(RankingWindow) == 1) return;
  view_dockwin("Ranking", RankingWindow, (_rankings = new MethodRanking()));

  EnterKeyListener* rankListener = new EnterKeyListener;
  _rankings->installEventFilter(rankListener);
  
  QObject::connect(rankListener, &EnterKeyListener::enterkey,      this, [&](){ add_hook(_rankings->getEnterMethod());});
  QObject::connect(_rankings,    &QListWidget::itemDoubleClicked,  this, [&](){ add_hook(_rankings->getEnterMethod());});
}

void FRMain::view_signals()
{
  if(_docks.count(SignalWindow) == 1) return;
  _descriptorsPerDock.erase(SignalWindow);
  _detailItems.erase(SignalWindow);
  view_dockwin("Signals", SignalWindow, (_list = new QListWidget()));
  QObject::connect(_list,  &QListWidget::itemDoubleClicked, this, &FRMain::show_detail);
}


void FRMain::view_timers()
{
  if(_docks.count(TimerWindow) == 1) return;
  _descriptorsPerDock.erase(TimerWindow);
  view_dockwin("Timers", TimerWindow, (_timedlist = new QListWidget()));
}


void FRMain::view_dockwin(const std::string& title, const std::string& dockname, QWidget* wptr)
{
  ActionBar* abar = new ActionBar(ActionBar::Close);
  _docks[dockname] = setup_dock_window(title, wptr, abar, "list_style");
  QObject::connect(abar, &ActionBar::close_clicked, _docks[dockname   ], &QDockWidget::close);
  QObject::connect(abar, &ActionBar::close_clicked, this,                [&](){_docks.erase(dockname);});
  restore_dock_win(dockname);
}


void FRMain::view_hooks_editor()
{
  if(_docks.count(EditHooksWindow) == 1) return;

  _editor = new frenchroast::Editor();
  ActionBar* abar = new ActionBar(ActionBar::Close | ActionBar::Save | ActionBar::Validate);
  QDockWidget* editdoc = setup_dock_window("hooks", _editor, abar, "edit_style");
  _docks[EditHooksWindow] = editdoc;
  restore_dock_win(EditHooksWindow);
  if(_hooksfile != "") {
    _editor->load_from_file(_hooksfile);
  }
  else {
    _settings.beginGroup("hooks");
    int total = _settings.beginReadArray("unlinked");
    for(int idx = 0; idx < total; idx++) {
      _settings.setArrayIndex(idx);
      _editor->add(_settings.value("item").toString());
    }
    _settings.endArray();
    _settings.endGroup();
  }


  
  QObject::connect(_editor,  &QTextEdit::destroyed,         this,    &FRMain::reset_editor);
  QObject::connect(abar,    &ActionBar::close_clicked,      editdoc, &QDockWidget::close);
  QObject::connect(abar,    &ActionBar::close_clicked,      this,    [&](){_docks.erase(EditHooksWindow);});
  QObject::connect(abar,    &ActionBar::save_clicked,       _editor, &frenchroast::Editor::save);
  QObject::connect(abar,    &ActionBar::validate_clicked,   _editor, &frenchroast::Editor::validate_hooks);
  QObject::connect(_editor, &frenchroast::Editor::saved ,   abar,    &ActionBar::disable_save);
  QObject::connect(_editor, &frenchroast::Editor::changed,  abar,    &ActionBar::enable_save);
  QObject::connect(_editor, &frenchroast::Editor::validated_hooks,  this,    &FRMain::validated_hooks);
}

void FRMain::add_hook(QString txt)
{
  if(_editor == nullptr) return;
  _editor->add_hook(txt);
}

QTableWidgetItem* createItem(int value)
{
  QTableWidgetItem* ditem = new QTableWidgetItem(QString::fromStdString(std::to_string(value)));
  ditem->setFont(CodeFont());
  ditem->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
  return ditem;
}

QTableWidgetItem* createItem(const std::string& value)
{
  QTableWidgetItem* ditem = new QTableWidgetItem(QString::fromStdString(value));
  ditem->setFont(CodeFont());
  ditem->setTextAlignment(Qt::AlignCenter);
  return ditem;
}

void FRMain::show_detail(QListWidgetItem* item)
{
  SignalItem* ptr = dynamic_cast<SignalItem*>(item);
  std::string sname = ptr->gettext();

  if(sname.find("[M]") == std::string::npos) {
      return;
  }

  _detailLists.erase(sname);
  _detailStackLists.erase(sname);
  _detailItems.erase(sname);
  _detailStackItems.erase(sname);
  
  _detailLists[sname] = new QTableWidget();
  _detailLists[sname]->setStyleSheet(_settings.value("traffic_grid_style").toString());

  _detailLists[sname]->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

  _detailLists[sname]->verticalHeader()->hide();
  _detailLists[sname]->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  
  ActionBar* abar = new ActionBar(ActionBar::Close);

  QWidget* holder = new QWidget();
  QVBoxLayout* vlayout = new QVBoxLayout();
  vlayout->setSpacing(0);
  vlayout->addWidget(_detailLists[sname] );
  vlayout->setContentsMargins(0,0,0,0);
  holder->setLayout(vlayout);
  holder->setStyleSheet(_settings.value("zero_border_style").toString());

  QWidget* holderStacks = new QWidget();
  vlayout = new QVBoxLayout();
  vlayout->setSpacing(0);
  _detailStackLists[sname] = new QTableWidget();
  _detailStackLists[sname]->setStyleSheet(_settings.value("traffic_grid_style").toString());
  _detailStackLists[sname]->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
  _detailStackLists[sname]->verticalHeader()->hide();
  _detailStackLists[sname]->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  _detailStackLists[sname]->insertColumn(0);
  _detailStackLists[sname]->insertColumn(0);
  _detailStackLists[sname]->setHorizontalHeaderItem(0, createItem("count"));
  _detailStackLists[sname]->setHorizontalHeaderItem(1,createItem("stack"));

  
  vlayout->addWidget(_detailStackLists[sname] );
  holderStacks->setLayout(vlayout);
  holderStacks->setStyleSheet(_settings.value("zero_border_style").toString());

  QTabWidget* tabs = new QTabWidget;
  tabs->addTab(holder, "Args / Instance data");
  tabs->addTab(holderStacks, "Call Stacks");
  
  QDockWidget* dockwin = setup_dock_window(sname, tabs, abar ,"table_style");
  QObject::connect(abar, &ActionBar::close_clicked, dockwin, &QDockWidget::close);
  QObject::connect(abar, &ActionBar::close_clicked, this, [=](){
                                                                 _detailLists.erase(sname);
                                                                 _detailStackLists.erase(sname);
                                                               });
  dockwin->setFloating(true);
  addDockWidget(Qt::TopDockWidgetArea, dockwin);
  update_detail_list(sname, _detailLists[ sname ], _detailStackLists[ sname ], _detailDescriptors[sname]);
}

void FRMain::reset_editor(QObject* obj)
{
   _editor = nullptr;
}

void FRMain::destroy_list(QObject* obj)
{
  if(_exit) return;
  std::string name = obj->objectName().toStdString();
  if(_detailItemsPerList.count(name) == 1) {
    for(auto& x : _detailItemsPerList[name]) {
      _detailItems.erase(x);
    }
    _detailItemsPerList.erase(name);
  }
  _detailLists.erase(name);
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
  if(_docks.count(ltype) == 0) return;
  
  if(markers.size() > 0) {
    descriptor = "[M] " + descriptor;
  }
  else {
    descriptor = "    " + descriptor;
  }
  tname = "[ " + tname + " ]";
  frenchroast::monitor::pad(descriptor, 50);
  frenchroast::monitor::pad(tname, 10);

  descriptor.append(tname);

  _detailDescriptors[descriptor] = DetailHolder{argHeaders, instanceHeaders, markers, stacks};
  if (_descriptorsPerDock[ltype].count(descriptor) == 0 ) {
    _descriptorsPerDock[ltype][descriptor] = new SignalItem(descriptor, frenchroast::monitor::ntoa(count,5,' '));
    _list->addItem(_descriptorsPerDock[ltype][descriptor]);
  }
  else {
    _descriptorsPerDock[ltype][descriptor]->setText(QString::fromStdString(descriptor) + "  " + QString::fromStdString(frenchroast::monitor::ntoa(count,5, ' ')));
    if(_detailLists.count(descriptor) == 1) {
      update_detail_list(descriptor, _detailLists[descriptor], _detailStackLists[descriptor], _detailDescriptors[descriptor]);
    }
  }
}


void FRMain::update_detail_list(std::string  descriptor, QTableWidget* list, QTableWidget* stacklist, const DetailHolder& detailHolder)
{
  for(auto& x : detailHolder._stacks) {
    if(_detailStackItems[descriptor].count(x.second.key()) == 1) {
      _detailStackItems[descriptor][x.second.key()]->setText(  QString::fromStdString(std::to_string(x.second.count())));
    }
    else {
      int row = stacklist->rowCount();
      stacklist->insertRow(row);
      QTableWidgetItem* item = createItem(x.second.count());
      _detailStackItems[descriptor][x.second.key()] = item;
      stacklist->setItem(row, 0, item);
      for(auto& frame : x.second.descriptors()) {
        stacklist->setItem(row,1, createItem(frame));
        ++row;
        stacklist->insertRow(row);
      }
    }
  }
  
  if(list->rowCount() == 0) {

    list->insertColumn(0);
    list->insertColumn(0);
    list->setHorizontalHeaderItem(0, createItem("invoked"));
    list->setHorizontalHeaderItem(1,createItem("("));
    
    int colidx = 2;
    for(auto& x : detailHolder._argHeaders) {
      list->insertColumn(colidx);
      list->setHorizontalHeaderItem(colidx++,createItem(x));
    }

    list->insertColumn(colidx);
    list->setHorizontalHeaderItem(colidx++,createItem(")"));

    for(auto& x : detailHolder._instanceHeaders) {
      list->insertColumn(colidx);
      list->setHorizontalHeaderItem(colidx++,createItem(x));
    }
  }


  for(auto& item : detailHolder._markers) {
  if(_detailItems[descriptor].count(item._descriptor) == 0 ) {
    if(item._count > 1) {
      int currRow = list->rowCount();
      list->insertRow(currRow);
      _detailItems[descriptor][item._descriptor] = currRow;
      list->setItem(currRow, 0, createItem(item._count));
      list->setItem(currRow, 1, createItem("("));
      
      int colidx = 2;
      for(auto& x : item._arg_items) {
        list->setItem(currRow, colidx++, createItem(x));
      }
      
      list->setItem(currRow, colidx, createItem(")"));
      ++colidx;
      for(auto& x : item._instance_items) {
        list->setItem(currRow, colidx++,createItem(x));
      }
    }
    else {
      _detailItems[descriptor][item._descriptor] = -1;
      int currRow = list->rowCount();
      if( _detailItems[descriptor].count("*") == 0) {
        list->insertRow(currRow);
        _detailItems[descriptor]["*"] = currRow;
        list->setItem(currRow, 0, createItem(1));
        list->setItem(currRow, 1, createItem("*"));
      }
      else {
        list->setItem(_detailItems[descriptor]["*"], 0, createItem(  list->item(_detailItems[descriptor]["*"],0)->text().toInt() + 1   ));
      }
    }
  }
  else {
    if(_detailItems[descriptor][item._descriptor] == -1  ) {
      if(item._count == 1) continue;
      QTableWidgetItem* titem = list->item(_detailItems[descriptor]["*"], 0);
      int total = titem->text().toInt() - 1;
      titem->setText( QString::number(total));
      int currRow = list->rowCount();
      list->insertRow(currRow);
      _detailItems[descriptor][item._descriptor] = currRow;
      list->setItem(currRow, 0, createItem(item._count));
      list->setItem(currRow, 1, createItem("("));
      int colidx = 2;
      for(auto& x : item._arg_items) {
        list->setItem(currRow, colidx++, createItem(x));
      }
      
      list->setItem(currRow, colidx, createItem(")"));
      ++colidx;
      for(auto& x : item._instance_items) {
        list->setItem(currRow, colidx++,createItem(x));
      }
    }
    else {
      list->item(_detailItems[descriptor][item._descriptor],0)->setText(  QString::fromStdString(std::to_string(item._count)) );
    }
  }
  }
}

std::string FRMain::format_markers(const std::string markers)
{
  using namespace frenchroast::monitor;
  using namespace frenchroast;
  std::string rv = "";
  for(auto& itempair : frenchroast::split(markers,";")) {
    if(itempair.find(":") == std::string::npos) continue;
    rv.append(  pad( split(itempair,":")[0] + "[ " + split(itempair,":")[1] + " ] ", 20));
  }
  return rv;
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
  if(_docks.count(TimerWindow) != 1) return;
  
  std::string desc{descriptor};
  tname = "[ " + tname + " ]";
  frenchroast::monitor::pad(desc, 50);
  frenchroast::monitor::pad(tname, 10);
  desc.append(tname);
  if (_descriptorsPerDock[TimerWindow].count(descriptor) == 0 ) {
    _descriptorsPerDock[TimerWindow][descriptor] = new SignalItem(desc, frenchroast::monitor::format_millis(elapsed));
    _timedlist->addItem(_descriptorsPerDock[TimerWindow][descriptor]);
  }
  else {
    _descriptorsPerDock[TimerWindow][descriptor]->setText(QString::fromStdString(desc) + "  " + QString::fromStdString(frenchroast::monitor::format_millis(elapsed)));
  }
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

  if(_editor != nullptr && _hooksfile == "") {
    _settings.beginGroup("hooks");
    _settings.remove("");
    _settings.beginWriteArray("unlinked");
    int idx = 0;
    for(auto& line : _editor->lines()) {
      _settings.setArrayIndex(idx++);
      _settings.setValue("item", QString::fromStdString(line));
    }
    _settings.endArray();
    _settings.endGroup();
  }
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
