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

#ifndef STACKROW_H
#define STACKROW_H


#include <QTableWidget>
#include <unordered_map>
#include <unordered_set>
#include "StackTrace.h"

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

#endif
