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

#include <iostream>
#include <QBoxLayout>
#include <QHeaderView>
#include <QSizePolicy>
#include <QMenu>
#include "FRStatus.h"
#include "MonitorUtil.h"
#include "QUtil.h"
#include "TimerItem.h"


FRStatus::FRStatus(QCheckBox* autocon) : _autoConnect(autocon)
{
    setStyleSheet("QWidget {background: #202020;color:#909090;font-family:\"Arial\";font-size:14px;}");
    _targets = new QTableWidget();
    _targets->setContextMenuPolicy(Qt::CustomContextMenu);
    QObject::connect(_targets, &QTableWidget::customContextMenuRequested, this, &FRStatus::show_menu);
    _targets->setStyleSheet("QTableWidget {border: 0px solid grey; background: black;color:#202020;font-size: 16px;font-family: \"Arial\";} ");
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
    auto* layout = new QVBoxLayout;
    layout->addWidget(_targets);
    layout->setContentsMargins(1,1,1,1);
    layout->setSpacing(0);
    setLayout(layout);
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



void display_row_connected(int row, QTableWidget* table)
{
  int totalcols = table->columnCount();
  for(int col = 0; col < totalcols; col++) {
    table->item(row, col)->setBackground(QBrush(QColor(40,108,34)));
  }
}

void display_row_disconnected(int row, QTableWidget* table)
{
  int totalcols = table->columnCount();
  for(int col = 0; col < totalcols; col++) {
    table->item(row, col)->setBackground(QBrush(QColor(90,90,90)));
    table->item(row, col)->setForeground(QBrush(QColor(200,200,200)));
  }
}

void display_row_dropped(int row, QTableWidget* table)
{
  int totalcols = table->columnCount();
  for(int col = 0; col < totalcols; col++) {
    table->item(row, col)->setBackground(QBrush(QColor(146,23,23)));
  }
}



void FRStatus::connect_client(bool forceDisplay)
{
    turn_on_profiler(_targets->item(_targets->currentRow(), 1)->text().toStdString() + _targets->item(_targets->currentRow(), 0)->text().toStdString());
    if(forceDisplay) {
      _clientStatus[_targets->currentRow()] = CONNECTED; 
      display_row_connected(_targets->currentRow(), _targets);
      dynamic_cast<TimerItem*>(_targets->item(_targets->currentRow(), 2))->start();
    }
}

void FRStatus::disconnect_client()
{
  turn_off_profiler(_targets->item(_targets->currentRow(), 1)->text().toStdString() + _targets->item(_targets->currentRow(), 0)->text().toStdString());
}

void FRStatus::remote_connected(const std::string& host, const std::string& pid)
{
}

void FRStatus::remote_disconnected(const std::string& host, const std::string& pid)
{
  _clientStatus[_targets->item(_items[host + pid ], 0)->row()] = DROPPED;
  display_row_dropped(_items[host + pid ], _targets);
  dynamic_cast<TimerItem*>(_targets->item(_items[host + pid ], 2))->stop();
}

void FRStatus::remote_ack_off(const std::string& host, const std::string& pid)
{
  _clientStatus[_targets->currentRow()] = NOT_CONNECTED; 
  display_row_disconnected(_targets->currentRow(), _targets);
  dynamic_cast<TimerItem*>(_targets->item(_targets->currentRow(), 2))->stop();
}

void FRStatus::remote_ack_on(const std::string& host, const std::string& pid)
{
  _clientStatus[_targets->currentRow()] = CONNECTED; 
  display_row_connected(_targets->currentRow(), _targets);
  dynamic_cast<TimerItem*>(_targets->item(_targets->currentRow(), 2))->start();
}


void FRStatus::remote_ready(const std::string& host, const std::string& pid)
{
  if(_items.count(host + pid) == 0) {
    _items[host + pid] = addRow(_targets, createItem(pid) , createItem(host), createItem(new TimerItem())  );
    _clientStatus[_targets->item(_items[host + pid ], 0)->row()] = NOT_CONNECTED;
    display_row_disconnected(_items[host + pid ], _targets);
    if(_autoConnect->checkState() == Qt::Checked) {
      _targets->setCurrentCell(_items[host + pid],0);
      connect_client(true);
    }
  }
}
