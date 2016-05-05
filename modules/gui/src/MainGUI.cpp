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
#include <QTCore/QObject>
#include <QThread>
#include <QHeaderView>

#include <winsock2.h>
#include <windows.h>
#include <iostream>
#include <ws2tcpip.h>
#include <sstream>
#include <unordered_map>
#include <string>
#include "fr.h"
#include "FRMain.h"

std::string ntoa(int x,int pad = 0)
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

void FRListener::signal_timed(const std::string& tag, long elapsed, int last) {
  timersignal("timer",tag,elapsed,last);
}

void FRListener::signal(const std::string& tag, int count) {
   QString qtag = QString::fromStdString(tag);
  thooked("signal",tag,count);
}

void FRListener::traffic(std::vector<frenchroast::monitor::StackTrace>& items)
{
  traffic_signal(items);
}

void FRListener::connected(const std::string& msg) {
  remoteconnected(msg);
  start_traffic(_trafficRate);
}

void FRListener::init()
{
  _mon.init_receiver(_ip, _port);
}

void FRListener::start_traffic(int rate)
{
  _mon.watch_traffic(rate);
}


QWidget* setup_list(const std::string title, QListWidget* list_ptr)
{
  list_ptr->setStyleSheet("QListWidget {border: 1px solid grey;background-color: black;font-size: 16px;font-family: \"Arial\"} QListWidget::item {color: orange;}");

  QWidget* holder = new QWidget;
  QVBoxLayout* vlayout = new QVBoxLayout();
  vlayout->setSpacing(0);
  holder->setLayout(vlayout);
  QLabel* sigLabel = new QLabel(QString::fromStdString(title));
  vlayout->addWidget(sigLabel);
  sigLabel->setStyleSheet("QLabel {border: 1px solid grey;qproperty-alignment: AlignHCenter;background-color: grey;font-size:16px;font-family: \"Arial\"}");
  vlayout->addWidget(list_ptr);
  return holder;
}



QWidget* build_traffic_viewer(QTableWidget* grid, QPushButton* bstart, QPushButton* bstop, QLineEdit* rate)
{
  QWidget* buttonHolder = new QWidget;
  QHBoxLayout* hlayout = new QHBoxLayout();
  QLabel* desc = new QLabel{QString::fromStdString("traffic rate (millisec):")};
  desc->setAlignment(Qt::AlignRight);
  
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


    
  QWidget* holder = new QWidget;
  QVBoxLayout* vlayout = new QVBoxLayout();
  vlayout->setSpacing(0);
  vlayout->addWidget(buttonHolder);
  holder->setLayout(vlayout);
  grid->setStyleSheet("QTableWidget {border: 1px solid grey;background-color: black;font-size: 16px;font-family: \"Arial\"} QTableWidget::item {color: orange;}");

  grid->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
  grid->horizontalHeader()->hide();
  grid->verticalHeader()->hide();
  grid->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  
  vlayout->addWidget(grid);
  return holder;
}

QWidget* setup_central(QWidget* lists, QTableWidget* grid, QPushButton* bstart, QPushButton* bstop, QLineEdit* rate)
{
  QWidget* holder = new QWidget;
  holder->setStyleSheet("QWidget {background-color: black;}");
  QVBoxLayout* vlayout = new QVBoxLayout();
  vlayout->setSpacing(0);
  holder->setLayout(vlayout);
  vlayout->addWidget(lists);
  vlayout->addWidget(build_traffic_viewer(grid,bstart,bstop,rate));
  
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
  layout->addWidget(setup_list("Signals", _list));
  layout->addWidget(setup_list("Timers", _timedlist));

  setCentralWidget(setup_central(datalists,_traffic,_buttonStartTraffic, _buttonStopTraffic,_rate));
  _traffic->insertColumn(0);
    
  QObject::connect(_traffic,&QTableWidget::itemDoubleClicked, this, &FRMain::show_deco);
  QObject::connect(_buttonStartTraffic, &QPushButton::clicked, this, &FRMain::update_traffic_rate);
  QObject::connect(this, &FRMain::start_traffic, listener, &FRListener::start_traffic);
  
  statusBar()->showMessage("waiting for connection...");
  QObject::connect(listener,&FRListener::remoteconnected, this, &FRMain::update_status);

}

void FRMain::update_traffic_rate()
{
  start_traffic(atoi(_rate->text().toStdString().c_str()));
}



void FRMain::update_status(std::string msg)
{
  statusBar()->showMessage(QString::fromStdString("remote agent connected: " + msg));
}

void FRMain::update_list(std::string ltype, std::string  descriptor, int count)
{
  if (_descriptors.count(descriptor) == 0 ) {
    _descriptors[descriptor] = new QListWidgetItem(QString::fromStdString(descriptor) + "  " + QString::fromStdString(ntoa(count)));
      _list->addItem(_descriptors[descriptor]);
  }
  else
    _descriptors[descriptor]->setText(QString::fromStdString(descriptor) + "  " + QString::fromStdString(ntoa(count)));
}



void FRMain::update_traffic(const std::vector<frenchroast::monitor::StackTrace>& stacks)
{
  for(auto& x : stacks) {
    if (_traffic_keys.count(x.key()) != 0) {
      continue;
    }
    int currRow = _traffic->rowCount();
    if(_traffic_keys.count(x.thread_name()) == 0) {
      _traffic->insertRow(currRow);
      StackRow* sr = new StackRow{x.thread_name(), currRow, _traffic};
      _traffic_keys[x.thread_name()] = sr;
    }
    _traffic_keys[x.thread_name()]->add(x);
  }
}

StackRow::StackRow(const std::string tname, int row, QTableWidget* tptr) : _tptr(tptr)
{

  _threadName = new FunctionPoint;
  _threadName->setBackground(QColor(64,64,64));
  _threadName->setText(QString::fromStdString(tname));
  _tptr->setItem(row, 0, _threadName);

}

void StackRow::add(const frenchroast::monitor::StackTrace& st)
{
  if (_keys.count(st.key()) != 0) return;

  // look for partial key
  for(auto itr = _keys.begin(); itr != _keys.end(); itr++) {
    if (st.key().find(   itr->first ) != std::string::npos) {
      //------ expand --
      int extra = st.frames().size() - _totalRows;
      for(int idx = 1; idx <= extra; idx++){
	_tptr->insertRow(_threadName->row() + _totalRows++);
      }
      //--------
      int col = 1;
      for(auto frameitr = itr->second.rbegin(); frameitr != itr->second.rend(); frameitr++   ) {
	_tptr->setItem((*frameitr)->row() + extra, (*frameitr)->column(), *frameitr);
      }
      int count = 0;
      std::string runningKey = st.key();
      for(int idx = st.frames().size() - 1 - extra; idx < st.frames().size(); idx++) {
	  FunctionPoint* fpitem = new FunctionPoint;
	  _keys[runningKey].push_back(fpitem);
	  fpitem->setText(QString::fromStdString(st.frames()[idx].get_name()  ));
	  fpitem->set_decorated_name(QString::fromStdString( st.frames()[idx].get_decorated_name()));
	  
	  _tptr->setItem(_threadName->row() - extra + count, _keys[st.key()][0]->column(), fpitem);
	  ++count;
	  _keys[runningKey + st.frames()[idx].get_decorated_name()] = _keys[runningKey];
	  runningKey += st.frames()[idx].get_decorated_name();

      }
      return; 
    }
  }

  // add new col
  _tptr->insertColumn(_tptr->columnCount());
    ++_col;      
  int count = 0;
  std::string runningKey = "";
  std::vector<FunctionPoint*> vec;
  
  int extra = st.frames().size() - _totalRows;
  for(int idx = 1; idx <= extra; idx++){
    _tptr->insertRow(_threadName->row() + _totalRows++);
  }

  for(auto& frame : st.frames()) {
    FunctionPoint* fpitem = new FunctionPoint;
    vec.push_back(fpitem);
    fpitem->setText(QString::fromStdString(frame.get_name()  ));
    fpitem->set_decorated_name(QString::fromStdString( frame.get_decorated_name()));
    if (count == 0) {
      fpitem->setBackground(QColor(64,64,64));
    }
    _tptr->setItem(_threadName->row() + count, _col, fpitem);
    ++count;
    runningKey += frame.get_decorated_name();
   _keys[runningKey] = vec;
  }

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

void FRMain::update_timed_list(std::string ltype, std::string  descriptor, long elapsed, int last)
{
  if (_descriptors.count(descriptor) == 0 ) {
    _descriptors[descriptor] = new QListWidgetItem(QString::fromStdString(descriptor) + "  " + QString::fromStdString(format_millis(elapsed)));
    _timedlist->addItem(_descriptors[descriptor]);
  }
  else
    _descriptors[descriptor]->setText(QString::fromStdString(descriptor) + "  " + QString::fromStdString(format_millis(elapsed)));
}



int main(int argc, char* argv[]) {
  qRegisterMetaType<std::string>();
  qRegisterMetaType< std::vector<frenchroast::monitor::StackTrace>>();
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

 tt->start();
 main.setWindowTitle("French Roast");
 main.show();

 return app.exec();
}



