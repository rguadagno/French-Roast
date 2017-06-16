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
#include <unordered_map>
#include <unordered_set>
#include "fr.h"



class FunctionPoint : public QTableWidgetItem {
  QString _name;

public:
  void set_decorated_name(const QString& name);
  QString get_name() const;
};



class StackRow {
  int _totalRows{1};
  int _col{1};
  FunctionPoint* _threadName;
  QTableWidget* _tptr;
  std::unordered_map<std::string, int>& _keys;
  std::unordered_set<std::string> _complete_keys;

  void add_column(const frenchroast::monitor::StackTrace& st, int col);
  void append_to_column(int col, const frenchroast::monitor::StackTrace& st);
 public:
  StackRow::StackRow(const std::string tname, int row, QTableWidget* tptr, std::unordered_map<std::string,int>& keys);
  FunctionPoint* thread_name();
  void add(const frenchroast::monitor::StackTrace& st);

};


class FRMain : public QMainWindow {
  Q_OBJECT

 public:
  FRMain(FRListener*);

 private:
  QListWidget* _list;
  QListWidget* _timedlist;
  QTableWidget* _traffic;
  QPushButton*  _buttonStartTraffic;
  QPushButton*  _buttonStopTraffic;
  QLineEdit*    _rate;
  std::unordered_map<std::string,QListWidgetItem*> _descriptors;
  std::unordered_map<std::string,StackRow*> _traffic_rows;
  std::unordered_map<std::string, int> _traffic_keys;


 signals:
   void start_traffic(int rate);
  
 public slots:
   void show_deco(QTableWidgetItem* item);
   void update_list(std::string, std::string,int);
   void update_timed_list(std::string ltype, std::string  descriptor, long elapsed, int last);
   void update_traffic(const std::vector<frenchroast::monitor::StackTrace>& stacks);
   void update_status(std::string);
   void update_unloaded_status(std::string);
   void update_traffic_rate();
};

class DetailsWin : public QMainWindow {
  Q_OBJECT

 public:


 private:

 signals:
  //void start_traffic(int rate);
  
 public slots:

   //   void update_list(std::string, std::string,int);

};


#endif
