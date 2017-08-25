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

#include <unordered_map>
#include <QLabel>
#include <QTableWidget>
#include <QCheckBox>


class FRStatus : public QWidget {

  Q_OBJECT

 private:
  QTableWidget*                        _targets;
  std::unordered_map<std::string, int> _items;
  enum STATUS {NOT_CONNECTED, CONNECTED, DROPPED};
  std::unordered_map<int, STATUS>      _clientStatus;
  QCheckBox*                           _autoConnect;

 public slots:
  void remote_connected(const std::string& host, const std::string& pid);
  void remote_disconnected(const std::string& host, const std::string& pid);
  void remote_ready(const std::string& host, const std::string& ip);
  void show_menu(const QPoint&);
  void connect_client( );
  void disconnect_client( );

 signals:
  void turn_on_profiler(const std::string& hostname_pid);
  void turn_off_profiler(const std::string& hostname_pid);
  
 public:
  FRStatus(QCheckBox*);
};

#endif
