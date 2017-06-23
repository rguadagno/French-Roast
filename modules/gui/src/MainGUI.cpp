// copyright (c) 2016 Richard Guadagno
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

#include <iostream>
#include <fstream>
#include <QTWidgets/QApplication>
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
#include <QThread>
#include <QHeaderView>
#include <QAction>

#include <winsock2.h>
#include <windows.h>
#include <iostream>
#include <ws2tcpip.h>
#include <sstream>
#include <unordered_map>
#include <string>
#include "fr.h"
#include "FRMain.h"



std::string ntoa(int x,int pad = 0);


class SignalItem : public QListWidgetItem {
  QString _text;

public:
  SignalItem(const QString& text, int total) : _text(text), QListWidgetItem( "  " + QString::fromStdString(ntoa(total)))
  {
  }


  ~SignalItem()
  {
        // qInfo("* DESTROYED");
  }

  
  
  const QString& gettext() const {
    return _text;
  }

};



std::string ntoa(int x,int pad )
  {
    std::stringstream ss;
    ss << x;
    std::string rv = ss.str();
    if (rv.length() < pad) {
      std::string pstr = "00000";
      rv = pstr.substr(0, pad - rv.length()) + rv;
    }
    return rv;
  }

FRListener::FRListener(const std::string ip, int port) : _ip(ip), _port(port), _mon(*this)
{
}

int FRListener::getCount(const std::string& item)
{
   return _items[item];
}

void FRListener::signal_timed(const std::string& tag, long elapsed, int last)
{
  timersignal(tag,elapsed);
}

void FRListener::signal(const std::string& tag, int count, std::vector<frenchroast::monitor::MarkerField>& markers)
{
  thooked("signal",tag,count, markers);
}

void FRListener::traffic(std::vector<frenchroast::monitor::StackTrace>& items)
{
  traffic_signal(items);
}

void FRListener::connected(const std::string& msg)
{
  remoteconnected("remote agent connected: " + msg);
  start_traffic(_trafficRate);
}

void FRListener::unloaded(const std::string& msg)
{
  remoteunloaded("remote agent disconnected: " + msg);
}

void FRListener::init()
{
  _mon.init_receiver(_ip, _port);
}

void FRListener::start_traffic(int rate)
{
  _mon.watch_traffic(rate);
}

void FRListener::stop_traffic()
{
  _mon.stop_watch_traffic();
}


QDockWidget* FRMain::setup_list(const std::string title, QListWidget* list_ptr, QDockWidget::DockWidgetFeatures features)
{
  list_ptr->setStyleSheet("QListWidget {border: 1px solid grey;background-color: black;font-size: 16px;font-family: \"Arial\"} QListWidget::item {color: orange;}");

  QDockWidget* holder = new QDockWidget(QString::fromStdString(title), this);
  holder->setStyleSheet("QWidget {border: 0px solid grey;background-color: grey;font-size:16px;font-family: \"Arial\"}");


  QLabel* sigLabel = new QLabel(QString::fromStdString(title));
  QWidget* titlebar = new QWidget();
  QBoxLayout* layout = new QBoxLayout(QBoxLayout::LeftToRight);
  layout->addWidget(sigLabel);
  if((features & QDockWidget::DockWidgetClosable) == QDockWidget::DockWidgetClosable) {
    QPushButton* btn = new QPushButton();
    QObject::connect(btn, &QPushButton::clicked, holder, &QDockWidget::close);
    btn->setIcon(style()->standardIcon(QStyle::SP_DockWidgetCloseButton));
    layout->addWidget(btn,1, Qt::AlignRight);
  }
  titlebar->setLayout(layout);
  sigLabel->setStyleSheet("QLabel {border: 1px solid grey;qproperty-alignment: AlignHCenter;color:#bababa;foreground-color:white;background-color: grey;font-size:16px;font-family: \"Arial\"}");
  holder->setTitleBarWidget(titlebar);
  holder->setAttribute(Qt::WA_DeleteOnClose);
  holder->setFeatures(features);
  QVBoxLayout* vlayout = new QVBoxLayout();
  holder->setLayout(vlayout);
  holder->setWidget(list_ptr);

  return holder;
}



QDockWidget* FRMain::build_traffic_viewer(QTableWidget* grid, QPushButton* bstart, QPushButton* bstop, QLineEdit* rate)
{
  QWidget* buttonHolder = new QWidget;
  QHBoxLayout* hlayout = new QHBoxLayout();
  QLabel* desc = new QLabel{QString::fromStdString("rate (millisec):")};
  desc->setAlignment(Qt::AlignRight);
  desc->setStyleSheet("QLabel {font-size:16px;font-family: \"Arial\"}");
  
  buttonHolder->setLayout(hlayout);
  rate->setText("100");
  bstart->setSizePolicy({QSizePolicy::Fixed,QSizePolicy::Fixed});
  bstop->setSizePolicy({QSizePolicy::Fixed,QSizePolicy::Fixed});
  rate->setSizePolicy({QSizePolicy::Fixed,QSizePolicy::Fixed});

  hlayout->addWidget(desc,0,Qt::AlignLeft);
  hlayout->addWidget(rate,0,Qt::AlignLeft);
  hlayout->addWidget(bstart,0,Qt::AlignLeft);
  
  buttonHolder->setStyleSheet("QWidget {border: 1px solid grey;background-color: grey;font-size: 16px;font-family: \"Arial\"} ");
  rate->setStyleSheet("QLineEdit {padding: 2px; border: 1px solid black;background-color: #C9C9C9;font-size: 16px;font-family: \"Arial\"}  ");

  rate->setFixedWidth(50);
  rate->setInputMask("0000");
  rate->setAlignment(Qt::AlignRight);

    bstart->setStyleSheet("QPushButton {padding: 2px; border: 1px solid black;background-color: #CF78CA;font-size: 16px;font-family: \"Arial\"} QPushButton::hover:!pressed {font-size: 20px;} ");
    bstart->setFixedWidth(70);


    hlayout->addWidget(bstop,1,Qt::AlignLeft);
    bstop->setStyleSheet("QPushButton {padding: 2px; border: 1px solid black;background-color: #8F6A82;font-size: 16px;font-family: \"Arial\"} QPushButton::hover:!pressed {font-size: 20px;} ");
    bstop->setFixedWidth(70);

    QDockWidget* docwin = new QDockWidget(QString::fromStdString("Traffic"), this);
    QWidget* holder = new QWidget();

    docwin->setWidget(holder);

    QVBoxLayout* vlayout = new QVBoxLayout();
    vlayout->setSpacing(0);
    holder->setLayout(vlayout);

   
  grid->setStyleSheet("QTableWidget {border: 0px solid grey;background-color: black;font-size: 16px;font-family: \"Arial\"} QTableWidget::item {color: orange;}");

  grid->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
  grid->horizontalHeader()->hide();
  grid->verticalHeader()->hide();
  grid->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  
  vlayout->addWidget(grid);

  QLabel* sigLabel = new QLabel(QString::fromStdString("Traffic"));
  QWidget* titlebar = new QWidget();
  QBoxLayout* layout = new QBoxLayout(QBoxLayout::LeftToRight);
  layout->setSpacing(0);
  layout->addWidget(buttonHolder,1,Qt::AlignLeft);
  layout->addWidget(sigLabel,1, Qt::AlignLeft);
  
  titlebar->setLayout(layout);
  sigLabel->setStyleSheet("QLabel {border: 1px solid grey;qproperty-alignment: AlignHCenter;color:#bababa;background-color: grey;font-size:16px;font-family: \"Arial\"}");
  docwin->setTitleBarWidget(titlebar);

  holder->setStyleSheet("QWidget {border: 0px solid grey;background-color: black;font-size:16px;font-family: \"Arial\"}");
  docwin->setStyleSheet("QWidget {border: 1px solid grey;background-color: grey;font-size:16px;font-family: \"Arial\"}");

  return docwin;
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


void FRMain::show_deco(QTableWidgetItem* item)
{
  QMessageBox box;
  FunctionPoint* ptr = dynamic_cast<FunctionPoint*>(item);
  box.setText(ptr->get_name());
  box.exec();
}

void FRMain::show_detail(QListWidgetItem* item)
{
  SignalItem* ptr = dynamic_cast<SignalItem*>(item);
  std::string sname{ptr->gettext().toStdString()};
  if(_detailLists.count(sname) > 0) {
    return;
  }
  _detailLists[sname] = new QListWidget(); 
  _detailLists[sname]->setObjectName(QString::fromStdString(sname));
  QObject::connect(_detailLists[sname], &QListWidget::destroyed, this, &FRMain::destroy_list);  
  addDockWidget(Qt::TopDockWidgetArea, setup_list(sname, _detailLists[sname]));
  update_detail_list(_detailLists[ sname ], _detailDescriptors[sname]);

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

FRMain::FRMain(FRListener* listener)
{
  resize(1300,300);
  _list      = new QListWidget;
  _timedlist = new QListWidget;
  _traffic   = new QTableWidget;
  _buttonStartTraffic = new QPushButton{"Start"};
  _buttonStopTraffic = new QPushButton{"Stop"};
  _rate = new QLineEdit;
  
  QWidget* datalists = new QWidget;
  QHBoxLayout* layout = new QHBoxLayout();
  datalists->setLayout(layout);
  datalists->setStyleSheet("QWidget {background-color: black;}");
  addDockWidget(Qt::TopDockWidgetArea, setup_list("Signals", _list, QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable));
  addDockWidget(Qt::TopDockWidgetArea, setup_list("Timers", _timedlist, QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable));

  addDockWidget(Qt::BottomDockWidgetArea, build_traffic_viewer(_traffic,_buttonStartTraffic,_buttonStopTraffic,_rate));
  
  _traffic->insertColumn(0);
  
  QObject::connect(_traffic,&QTableWidget::itemDoubleClicked, this, &FRMain::show_deco);
  QObject::connect(_list,   &QListWidget::itemDoubleClicked, this, &FRMain::show_detail);
  QObject::connect(_buttonStartTraffic, &QPushButton::clicked, this, &FRMain::update_traffic_rate);
  QObject::connect(_buttonStopTraffic, &QPushButton::clicked, listener, &FRListener::stop_traffic);
  QObject::connect(this, &FRMain::start_traffic, listener, &FRListener::start_traffic);
  
  statusBar()->showMessage("waiting for connection...");

  QObject::connect(listener,&FRListener::remoteconnected, this, &FRMain::update_status);
  QObject::connect(listener,&FRListener::remoteunloaded, this, &FRMain::update_unloaded_status);

}

void FRMain::update_traffic_rate()
{
  start_traffic(atoi(_rate->text().toStdString().c_str()));
}



void FRMain::update_status(std::string msg)
{
  statusBar()->showMessage(QString::fromStdString(msg));
}

void FRMain::update_unloaded_status(std::string msg)
{
  statusBar()->setStyleSheet("QWidget {background-color: #ab1e1e;}");
  statusBar()->showMessage(QString::fromStdString(msg));
}


void FRMain::update_list(std::string ltype, std::string  descriptor, int count, const std::vector<frenchroast::monitor::MarkerField>& markers)
{
  _detailDescriptors[descriptor] = markers;
  if (_descriptors.count(descriptor) == 0 ) {
    _descriptors[descriptor] = new SignalItem(QString::fromStdString(descriptor), count);
    _list->addItem(_descriptors[descriptor]);
  }
  else {
    _descriptors[descriptor]->setText(QString::fromStdString(descriptor) + "  " + QString::fromStdString(ntoa(count)));
      if(_detailLists.count(descriptor) == 1) { 
        update_detail_list(_detailLists[descriptor], markers);
      }
  }
}

void FRMain::update_detail_list(QListWidget* list, const std::vector<frenchroast::monitor::MarkerField>& markers)
{
  for(auto& item : markers) {
    if(_detailItems.count(item._descriptor) == 0) {
      _detailItems[item._descriptor] = new QListWidgetItem(QString::fromStdString(item._descriptor) + "  " + QString::fromStdString(ntoa(item._count)));
      list->addItem(_detailItems[item._descriptor]);
      _detailItemsPerList[ list->objectName().toStdString() ].push_back(item._descriptor);
    }
    else {
      _detailItems[item._descriptor]->setText(QString::fromStdString(item._descriptor) + "  " + QString::fromStdString(ntoa(item._count)));      
    }
  }
}

void FRMain::update_traffic(const std::vector<frenchroast::monitor::StackTrace>& stacks)
{
  for(auto& x : stacks) {
    if (_traffic_keys.count(x.key()) != 0) {
      continue;
    }
    int currRow = _traffic->rowCount();
    if(_traffic_rows.count(x.thread_name()) == 0) {
      _traffic->insertRow(currRow);
      StackRow* sr = new StackRow{x.thread_name(), currRow, _traffic, _traffic_keys};
      _traffic_rows[x.thread_name()] = sr;
    }
    _traffic_rows[x.thread_name()]->add(x);
  }
}

StackRow::StackRow(const std::string tname, int row, QTableWidget* tptr, std::unordered_map<std::string,int>& keys) : _tptr(tptr), _keys(keys)
{
  _threadName = new FunctionPoint;
  _threadName->setBackground(QColor(64,64,64));
  _threadName->setText(QString::fromStdString(tname));
  _tptr->setItem(row, 0, _threadName);
}

FunctionPoint* StackRow::thread_name()
{
  return _threadName;
}

void StackRow::append_to_column(int col, const frenchroast::monitor::StackTrace& st)
{
  add_column(st,col);
}

void StackRow::add_column(const frenchroast::monitor::StackTrace& st, int col)
{
  int count = 0;
  std::string runningKey = "";
  int extra = st.frames().size() - _totalRows;
      for(int idx = 1; idx <= extra; idx++){
	_tptr->insertRow(_threadName->row() + _totalRows++);
      }
      
      for(auto& frame : st.frames()) {
	FunctionPoint* fpitem = new FunctionPoint;
	fpitem->setText(QString::fromStdString(frame.get_name()  ));
	fpitem->set_decorated_name(QString::fromStdString( frame.get_decorated_name()));
	if (count == 0) {
	  fpitem->setBackground(QColor(64,64,64));
	}
	_tptr->setItem(_threadName->row() + count, col, fpitem);
	++count;
	runningKey += frame.get_decorated_name();
	_keys[runningKey] = col;
      }
}

void StackRow::add(const frenchroast::monitor::StackTrace& st)
{
  if (_keys.count(st.key()) != 0) return;

  std::string partialkey = "";
  for(auto& x : _complete_keys) {
    if (st.key().find(x) != std::string::npos) {
      partialkey = x;
    }
  }
  
  if (partialkey != "") {
    append_to_column(_keys[partialkey], st);
  }
  else {
    ++_col;
    if (_tptr->columnCount() < _col) {
      _tptr->insertColumn(_tptr->columnCount());
    }
    add_column(st, _col -1 );
  }
  
  _complete_keys.clear();
  _complete_keys.insert(st.key());
  
  }




std::string format_millis(long millis)
{
  int hours = millis / (3600000);
  millis -= hours * 3600000;
  int min   = millis / 60000;
  millis -= min * 60000;
  int sec   = millis / 1000;
  millis -=  sec * 1000;

  return ntoa(hours) + ":" + ntoa(min,2) + ":" + ntoa(sec,2) + ":" + ntoa(millis,3);
}

void FRMain::update_timed_list(std::string  descriptor, long elapsed)
{
  if (_descriptors.count(descriptor) == 0 ) {
    _descriptors[descriptor] = new QListWidgetItem(QString::fromStdString(descriptor) + "  " + QString::fromStdString(format_millis(elapsed)));
    _timedlist->addItem(_descriptors[descriptor]);
  }
  else
    _descriptors[descriptor]->setText(QString::fromStdString(descriptor) + "  " + QString::fromStdString(format_millis(elapsed)));
}

void FRMain::handle_exit()
{
  _exit = true;
}

int main(int argc, char* argv[]) {
  qRegisterMetaType<std::string>();
  qRegisterMetaType<std::vector<frenchroast::monitor::StackTrace>>();
  qRegisterMetaType<std::vector<frenchroast::monitor::MarkerField>>();
  
  QApplication app(argc,argv);

  if (argc != 3) {
    QMessageBox box;
    box.setText("usage: roaster IP PORT\nexample: roaster 127.0.0.1 6060");
    box.exec();
    exit(0);
  }

  FRListener roaster{std::string{argv[1]}, atoi(argv[2])};
  QThread* tt = new QThread(&roaster);

  roaster.moveToThread(tt);
  
  FRMain main(&roaster);

  QObject::connect(&roaster,&FRListener::thooked, &main, &FRMain::update_list);
  QObject::connect(&roaster,&FRListener::timersignal, &main, &FRMain::update_timed_list);
  QObject::connect(&roaster,&FRListener::traffic_signal, &main, &FRMain::update_traffic);
  QObject::connect(tt,&QThread::started, &roaster, &FRListener::init);
  QObject::connect(&app, &QApplication::aboutToQuit, &main, &FRMain::handle_exit);

  
  tt->start();
  main.setWindowTitle("French Roast");
  main.show();

 return app.exec();
}



