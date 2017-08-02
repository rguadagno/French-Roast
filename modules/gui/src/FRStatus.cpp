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
#include <QBoxLayout>


FRStatus::FRStatus() 
{
  _statusText = new QLabel;
  _statusText->setFixedHeight(24);
  _statusText->setStyleSheet("QLabel {padding-left:50px;}");
    
  _connectionText = new QLabel;
  _connectionText->setFixedHeight(24);
  _connectionText->setStyleSheet("QLabel {padding-left:2px;}");

  _timeText = new QLabel;
  _timeText->setFixedHeight(24);
  _timeText->setStyleSheet("QLabel {padding-right:2px;}");
  _timeText->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
  
  _timer   = new QTimer{this};
  using namespace frenchroast::monitor;
  QObject::connect(_timer,
                   &QTimer::timeout,
                   _timeText,
                   [&]() { _elapsed+= 1000; _timeText->setText(QString::fromStdString(format_millis(_elapsed, FORMAT_HOURS|FORMAT_MINUTES|FORMAT_SECONDS))); });
  
  auto* layout = new QHBoxLayout;
  layout->addWidget(_connectionText);
  layout->addWidget(_statusText);
  layout->addWidget(_timeText, Qt::AlignRight);
  layout->setContentsMargins(1,1,1,1);
  layout->setSpacing(0);
  setLayout(layout);
  setStyleSheet("QLabel {border:none; background:#c8c8c8;color:black;font-family:\"Arial\";font-size:12px;}");
}

void FRStatus::waiting_for_connection()
{
  _connectionText->setText("waiting for connection...");
}

void FRStatus::remote_connected(const std::string& from)
{
  _connectionText->setText(QString::fromStdString("remote agent connected: " +  from));
  setStyleSheet("QWidget {border:none; background: #286c22;color:#c8c8c8;font-family:\"Arial\";font-size:12px;}");
  _statusText->setText("( Waiting for Signal Validation )");
}

void FRStatus::remote_disconnected(const std::string& msg)
{
  _connectionText->setText(QString::fromStdString("remote agent disconnected: " +  msg));
   setStyleSheet("QWidget {border:none; background-color: #d04949;color:white;font-family:\"Arial\";font-size:12px;}");
  _statusText->clear();
  _timer->stop();
}


void FRStatus::remote_ready()
{
  _elapsed = 0;
  _timer->start(1000);
  _statusText->setText("( Running )");
}
