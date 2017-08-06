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
#include <QSizePolicy>
#include <QMenu>
#include <iostream>


FRStatus::FRStatus() 
{
    _targets = new QTableWidget();
    _targets->setContextMenuPolicy(Qt::CustomContextMenu);
    QObject::connect(_targets, &QTableWidget::customContextMenuRequested, this, &FRStatus::show_menu);
    _targets->setStyleSheet("QTableWidget {border: 0px solid grey; background: black;font-size: 16px;font-family: \"Arial\"} ");
    _targets->verticalHeader()->hide();
    _targets->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    _targets->insertColumn(0);
    _targets->insertColumn(0);
    _targets->insertColumn(0);
    _targets->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    _targets->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    _targets->setHorizontalHeaderItem(0,createItem("PID"));
    _targets->setHorizontalHeaderItem(1, createItem("host"));
    _targets->setHorizontalHeaderItem(2, createItem("elapsed time connected"));
    _targets->horizontalHeader()->setStyleSheet("QWidget {background: #606060;} ");
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

void FRStatus::show_menu(const QPoint& pos)
{
  QMenu* menu = new QMenu();
  STATUS srow = _clientStatus[_targets->currentRow()];
  if( srow == NOT_CONNECTED ) {
    QObject::connect(menu->addAction("Connect"), &QAction::triggered, this, &FRStatus::connect_client);
  }

  if(srow == DROPPED) {
    QObject::connect(menu->addAction("Acknowledge"), &QAction::triggered, this, [&]() { _targets->setRowHidden(_targets->currentRow(),true); });
  }

  if(srow == CONNECTED) {
    QObject::connect(menu->addAction("Disconnect"), &QAction::triggered, this, &FRStatus::disconnect_client);
  }

  menu->exec(mapToGlobal(pos));
  
  delete menu;

}


void FRStatus::connect_client()
{
  _clientStatus[_targets->currentRow()] = CONNECTED; // fix this, msut wait for client to ACK
  turn_on_profiler(_targets->item(_targets->currentRow(), 1)->text().toStdString() + _targets->item(_targets->currentRow(), 0)->text().toStdString());
  
  _targets->item(_targets->currentRow(), 0)->setBackground(QBrush(QColor(40,108,34)));
  _targets->item(_targets->currentRow(), 1)->setBackground(QBrush(QColor(40,108,34)));
}

void FRStatus::disconnect_client()
{
  _clientStatus[_targets->currentRow()] = NOT_CONNECTED; // fix this, msut wait for client to ACK
  turn_off_profiler(_targets->item(_targets->currentRow(), 1)->text().toStdString() + _targets->item(_targets->currentRow(), 0)->text().toStdString());
  
  _targets->item(_targets->currentRow(), 0)->setForeground(QBrush(QColor(0,0,0)));
  _targets->item(_targets->currentRow(), 1)->setForeground(QBrush(QColor(200,200,200)));
  _targets->item(_targets->currentRow(), 0)->setBackground(QBrush(QColor(90,90,90)));
  _targets->item(_targets->currentRow(), 1)->setBackground(QBrush(QColor(90,90,90)));

}


void FRStatus::waiting_for_connection()
{
  //  _connectionText->setText("waiting for connection...");
}

void FRStatus::remote_connected(const std::string& host, const std::string& pid)
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

void FRStatus::remote_disconnected(const std::string& host, const std::string& pid)
{
  _clientStatus[_targets->item(_items[host + pid ], 0)->row()] = DROPPED;
  _targets->item(_items[host + pid ], 0)->setBackground(QBrush(QColor(165,13,13)));
  _targets->item(_items[host + pid ], 1)->setBackground(QBrush(QColor(165,13,13)));
  //  _targets->item(_items[host + pid ], 1)->setBackground(QBrush(QColor(96,96,96)));
  //_timer->stop();
}


void FRStatus::remote_ready(const std::string& host, const std::string& pid)
{
  _elapsed = 0;

  if(_items.count(host + pid) == 0) {
    _items[host + pid] = addRow(_targets, createItem(pid) , createItem(host)  );
    std::cout << " ADDED ROW: " << _items[host + pid ] << std::endl;
    //QBrush(QColor(40,108,34)));
    _clientStatus[_targets->item(_items[host + pid ], 0)->row()] = NOT_CONNECTED;
    _targets->item(_items[host + pid ], 0)->setBackground(QBrush(QColor(90,90,90)));
    _targets->item(_items[host + pid ], 0)->setForeground(QBrush(QColor(200,200,200)));

    _targets->item(_items[host + pid ], 1)->setBackground(QBrush(QColor(90,90,90)));
    _targets->item(_items[host + pid ], 1)->setForeground(QBrush(QColor(200,200,200)));

  }
  
  //_timer->start(1000);
  //_statusText->setText("( Running )");
}
