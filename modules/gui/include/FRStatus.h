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

#ifndef FRSTATUS_H
#define FRSTATUS_H

#include <QLabel>
#include <QStatusBar>
#include <QTimer>

class FRStatus : public QWidget {

  Q_OBJECT

 private:
  QStatusBar* _Qstatusbar;
  QLabel*     _connectionText;
  QLabel*     _statusText;
  QLabel*     _timeText;
  QTimer*     _timer;
  int         _elapsed{0};

 public slots:
  void remote_connected(const std::string& from);
  void remote_disconnected(const std::string& msg);
  void remote_ready();
 public:
  FRStatus(QStatusBar* ptr);
  void waiting_for_connection();
};

#endif
