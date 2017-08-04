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

#include "FRStatus.h"
#include "MonitorUtil.h"
#include "QUtil.h"
#include <QBoxLayout>
#include <QHeaderView>


FRStatus::FRStatus() 
{

    _targets = new QTableWidget();
    _targets->setStyleSheet("QTableWidget {border: 0px solid grey; background: black;font-size: 16px;font-family: \"Arial\"} " \
                            "QTableWidget::item {background:#286c22; color:#c8c8c8;}");


    _targets->verticalHeader()->hide();
    _targets->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    _targets->insertColumn(0);
    _targets->insertColumn(0);
    _targets->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    _targets->setHorizontalHeaderItem(0, createItem("host"));
    _targets->setHorizontalHeaderItem(1,createItem("PID"));
    _targets->horizontalHeader()->setStyleSheet("QWidget {background: #606060;}");

    auto* layout = new QHBoxLayout;
    layout->addWidget(_targets);
    layout->setContentsMargins(1,1,1,1);
    layout->setSpacing(0);
    setLayout(layout);
  /*
  _timer   = new QTimer{this};
  using namespace frenchroast::monitor;
  QObject::connect(_timer,
                   &QTimer::timeout,
                   _timeText,
                   [&]() { _elapsed+= 1000; _timeText->setText(QString::fromStdString(format_millis(_elapsed, FORMAT_HOURS|FORMAT_MINUTES|FORMAT_SECONDS))); });
  */
}

void FRStatus::waiting_for_connection()
{
  //  _connectionText->setText("waiting for connection...");
}

void FRStatus::remote_connected(const std::string& from)
{

  /*
  if(_items.count("from") == 0) {
    _items[from] = createItem(from);
    addRow(_targets, _items[from]);
  }
  else {
    _items[from]->setText(QString::fromStdString(from));
  }
  */
  //  _connectionText->setText(QString::fromStdString("remote agent connected: " +  from));
  //setStyleSheet("QWidget {border:none; background: #286c22;color:#c8c8c8;font-family:\"Arial\";font-size:12px;}");
  //  _statusText->setText("( Waiting for Signal Validation )");
}

void FRStatus::remote_disconnected(const std::string& msg)
{
  //_connectionText->setText(QString::fromStdString("remote agent disconnected: " +  msg));
  // setStyleSheet("QWidget {border:none; background-color: #d04949;color:white;font-family:\"Arial\";font-size:12px;}");
   //  _statusText->clear();
  //_timer->stop();
}


void FRStatus::remote_ready(const std::string& host, const std::string& ip)
{
  _elapsed = 0;

  if(_items.count(host+ip) == 0) {
    _items[host+ip] = createItem("[ " + ip + " ]");
    addRow(_targets, createItem(host), _items[host+ip] );
  }
  
  //_timer->start(1000);
  //_statusText->setText("( Running )");
}
