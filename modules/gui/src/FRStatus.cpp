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


FRStatus::FRStatus(QStatusBar* ptr) : _Qstatusbar(ptr)
{
}

void FRStatus::waiting_for_connection()
{
  _Qstatusbar->setStyleSheet("QWidget{background-color: #404040;}");
  setStyleSheet("QWidget{border:0px;background-color: #404040;color:white;font-family:\"Arial\";font-size:12px;}");
  setText("waiting for connection...");
}

void FRStatus::remote_connected(const std::string& from)
{
  _Qstatusbar->setStyleSheet("QWidget{background-color: #68a34d;}");
  setStyleSheet("QLabel {background-color: #68a34d;color:white;font-family:\"Arial\";font-size:12px;}");
  setText(QString::fromStdString("remote agent connected: " +  from));

}

void FRStatus::remote_disconnected(const std::string& msg)
{
  _Qstatusbar->setStyleSheet("QWidget{background-color:#d04949;}");
  setStyleSheet("QLabel {background-color: #d04949;color:white;font-family:\"Arial\";font-size:12px;}");
  setText(QString::fromStdString("remote agent disconnected: " +  msg));
}

