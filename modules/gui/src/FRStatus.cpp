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
#include <QGridLayout>


FRStatus::FRStatus(QStatusBar* ptr) : _Qstatusbar(ptr)
{
  _statusText = new QLabel;
  _statusText->setStyleSheet("QLabel {color:#c8c8c8;font-family:\"Arial\";font-size:12px;}");
  _connectionText = new QLabel;
  _timeText = new QLabel;
  _timer   = new QTimer{this};
  QObject::connect(_timer, &QTimer::timeout, _timeText, [&]() { _elapsed+= 1000; _timeText->setText(QString::fromStdString(frenchroast::monitor::format_millis(_elapsed))); });
  QGridLayout* layout = new QGridLayout;
  layout->addWidget(_connectionText, 0,0, Qt::AlignLeft);
  layout->addWidget(_statusText,0,7, Qt::AlignCenter);
  layout->addWidget(new QLabel("Elapsed:"), 0,10, Qt::AlignRight);
  layout->addWidget(new QLabel(""), 0,9);
  layout->setColumnStretch(9,10);
  layout->setColumnStretch(7,8);
  layout->addWidget(_timeText, 0,11, Qt::AlignRight);
  layout->setContentsMargins(1,1,1,1);
  setLayout(layout);
}

void FRStatus::waiting_for_connection()
{
  _Qstatusbar->setStyleSheet("QWidget{background-color: #404040;}");
  setStyleSheet("QWidget{border:0px;background-color: #404040;color:white;font-family:\"Arial\";font-size:12px;}");
  _connectionText->setText("waiting for connection...");
}

void FRStatus::remote_connected(const std::string& from)
{
  _Qstatusbar->setStyleSheet("QWidget{background-color: #286c22;}");
  setStyleSheet("QLabel {background-color: #286c22;color:white;font-family:\"Arial\";font-size:12px;}");
  _connectionText->setText(QString::fromStdString("remote agent connected: " +  from));
  _timeText->setText("elapsed: ");
  _statusText->setText("( Waiting for Signal Validation )");

}

void FRStatus::remote_disconnected(const std::string& msg)
{
  _Qstatusbar->setStyleSheet("QWidget{background-color:#d04949;}");
  setStyleSheet("QLabel {background-color: #d04949;color:white;font-family:\"Arial\";font-size:12px;}");
  _connectionText->setText(QString::fromStdString("remote agent disconnected: " +  msg));
  _statusText->clear();
  _timer->stop();
}


void FRStatus::remote_ready()
{
  _elapsed = 0;
  _timer->start(1000);
  _statusText->setText("( Running )");
}
