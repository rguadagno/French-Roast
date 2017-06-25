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

#ifndef MAINWIN_H
#define MAINWIN_H

#include <QMainWindow>
#include <QLabel>
#include <QListWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QDockWidget>
#include <QSettings>
#include <unordered_map>
#include <unordered_set>
#include "fr.h"
#include "StackRow.h"


class SignalItem;

class FRMain : public QMainWindow {
  Q_OBJECT

 public:
  FRMain(FRListener*, QSettings&);

 private:
  QSettings&    _settings;
  bool          _exit{false};
  QListWidget*  _list;
  QListWidget*  _timedlist;
  QTableWidget* _traffic;
  QPushButton*  _buttonStartTraffic;
  QPushButton*  _buttonStopTraffic;
  QLineEdit*    _rate;
  std::unordered_map<std::string,SignalItem*>        _descriptors;
  std::unordered_map<std::string,SignalItem*>         _detailItems;
  std::unordered_map<std::string,QListWidget*>        _detailLists;
  std::unordered_map<std::string, std::vector<std::string>> _detailItemsPerList;
  std::unordered_map<std::string, std::vector<frenchroast::monitor::MarkerField>> _detailDescriptors;
  std::unordered_map<std::string,StackRow*> _traffic_rows;
  std::unordered_map<std::string, int> _traffic_keys;
  
  QDockWidget* setup_list(const std::string title, QListWidget* list_ptr,
                          QDockWidget::DockWidgetFeatures features = QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
  void update_detail_list(QListWidget*, const std::vector<frenchroast::monitor::MarkerField>& markers);
  QDockWidget* build_traffic_viewer(QTableWidget* grid, QPushButton* bstart, QPushButton* bstop, QLineEdit* rate);
 signals:
   void start_traffic(int rate);
  
 public slots:
   void show_deco(QTableWidgetItem* item);
   void show_detail(QListWidgetItem* item);
   void destroy_list(QObject* obj);
   void handle_exit();
   void update_list(std::string, std::string, std::string,int, const std::vector<frenchroast::monitor::MarkerField>&);
   void update_timed_list(std::string  descriptor, std::string, long elapsed);
   void update_traffic(const std::vector<frenchroast::monitor::StackTrace>& stacks);
   void update_status(std::string);
   void update_unloaded_status(std::string);
   void update_traffic_rate();
};



#endif
