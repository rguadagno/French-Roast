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
#include <QTWidgets/QTableWidget>
#include <QTCore/QObject>
#include <QThread>

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


FRListener::FRListener() : _mon(*this)
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

void FRListener::connected(const std::string& msg) {
  remoteconnected(msg);
}

void FRListener::init()
{
  frenchroast::monitor::Monitor<FRListener> mon{*this};
 _mon.init_receiver("10.0.1.199",6060);
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


QWidget* build_traffic_viewer(QTableWidget* grid)
{
  QWidget* holder = new QWidget;
  QVBoxLayout* vlayout = new QVBoxLayout();
  vlayout->setSpacing(0);
  holder->setLayout(vlayout);
  grid->setStyleSheet("QTableWidget {border: 1px solid grey;background-color: black;font-size: 16px;font-family: \"Arial\"} QListWidget::item {color: orange;}");

  vlayout->addWidget(grid);
  
  
  

  return holder;
}

QWidget* setup_central(QWidget* lists, QTableWidget* grid)
{
  QWidget* holder = new QWidget;
  holder->setStyleSheet("QWidget {background-color: black;}");
  QVBoxLayout* vlayout = new QVBoxLayout();
  vlayout->setSpacing(0);
  holder->setLayout(vlayout);
  vlayout->addWidget(lists);
  vlayout->addWidget(build_traffic_viewer(grid));
  
  return holder;
}



//class FR
/*
  map<name, vector<vector<string>>
>    somefunc1:somefunc5:somefunc7
>    somefunc1:somefunc8:...

     somefunc1 -->  somefunc5
     somefunc1 --> somefunc8 



 */

class FunctionPoint : public QTableWidgetItem {
  QString _name;
public:
  void set_decorated_name(const QString& name)
  {
    _name = name;
  }
  QString get_name() const
  {
    return _name;
  }
};

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
  _list = new QListWidget;
  _timedlist = new QListWidget;
  _traffic = new QTableWidget;
  
  QWidget* datalists = new QWidget;
  QHBoxLayout* layout = new QHBoxLayout();
  datalists->setLayout(layout);
  datalists->setStyleSheet("QWidget {background-color: black;}");
  layout->addWidget(setup_list("Signals", _list));
  layout->addWidget(setup_list("Timers", _timedlist));


  //setCentralWidget(datalists);
  setCentralWidget(setup_central(datalists,_traffic));
  FunctionPoint* item = new FunctionPoint;
  item->setForeground(QBrush{Qt::red});
  item->setFont(QFont{"Courier"});
  item->setText(QString{"somefunc() -->"});
  item->set_decorated_name(QString{"somefunc(int)string -->"});
  
  //  _traffic->insertRow(0);
  for(int idx = 1; idx <= 10; idx++) {
    _traffic->insertColumn(0);
  }
  
  //  _traffic->setItem(0,1,item);

  QObject::connect(_traffic,&QTableWidget::itemDoubleClicked, this, &FRMain::show_deco);
  
  statusBar()->showMessage("waiting for connection...");
  QObject::connect(listener,&FRListener::remoteconnected, this, &FRMain::update_status);
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



void FRMain::update_traffic(std::vector<frenchroast::monitor::StackTrace> stacks)
{
  for(auto& x : stacks) {
    if (_traffic_keys.count(x.key()) == 0) {
      int currRow = _traffic->rowCount();
      if(_traffic_keys.count(x.thread_name() == 0)) {
	_traffic->insertRow(currRow);
	FunctionPoint* tname = new FunctionPoint;
	tname->setText(QString{x.thread_name()});
	_traffic->insertItem(currRow,0, tname);
	_traffic_keys[x.thread_name()] = tname;
      }
      else {
	currRow = _traffic_keys[x.thread_name()].row() + 1;
	_traffic->insertRow(currRow);
      }
      std::string runningKey = x.thread_name();
      for(auto& fp : x.frames()) {
	if (_traffic_keys.count(runningKey + fp.key()) == 0) {
	  FunctionPoint* fpitem = new FunctionPoint;
	  fpitem->setText(fp.get_name());
	  fpitem->set_decorated_name(fp.get_decorated_name());
	  _traffic.insertItem(currRow, _traffic_keys[runningKey].column() + 1, fpitem);
	  _traffic_keys[runningKey + fp.key()] = fpitem;
	}
	  runningKey += fp.key();
      }

    }
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
  QApplication app(argc,argv);

  FRListener roaster;
  QThread* tt = new QThread(&roaster);

  roaster.moveToThread(tt);

 FRMain main(&roaster);
 QObject::connect(&roaster,&FRListener::thooked, &main, &FRMain::update_list);
 QObject::connect(&roaster,&FRListener::timersignal, &main, &FRMain::update_timed_list);
 QObject::connect(&roaster,&FRListener::traffic_arrived, &main, &FRMain::update_traffic);
 
 QObject::connect(tt,&QThread::started, &roaster, &FRListener::init);

 tt->start();
 main.setWindowTitle("French Roast");
 main.show();

 return app.exec();
}



