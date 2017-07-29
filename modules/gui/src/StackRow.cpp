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

#include "StackRow.h"
#include "Codefont.h"
#include <iostream>

QFont StackRow::_font = CodeFont();

StackRow::StackRow(const std::string tname, int row, QTableWidget* tptr, std::unordered_map<std::string,int>& keys) : _tptr(tptr), _keys(keys)
{
  _threadName = new FunctionPoint;
  _threadName->setFont(_font);
  _threadName->setBackground(QColor(64,64,64));
  _threadName->setText(QString::fromStdString(tname));
  _tptr->setItem(row, 0, _threadName);
}

FunctionPoint* StackRow::thread_name()
{
  return _threadName;
}

void StackRow::append_to_column(int col, const frenchroast::monitor::StackTrace& st)
{
  add_column(st,col);
}

void StackRow::add_column(const frenchroast::monitor::StackTrace& st, int col)
{
  int count = 0;
  std::string runningKey = "";
  int extra = st.frames().size() - _totalRows;
  for(int idx = 1; idx <= extra; idx++){
    _tptr->insertRow(_threadName->row() + _totalRows++);
  }
      
  for(auto& frame : st.frames()) {
    FunctionPoint* fpitem = new FunctionPoint;
    fpitem->setFont(_font);
    fpitem->setText(QString::fromStdString(frame.get_name()  ));
    fpitem->set_decorated_name(QString::fromStdString( frame.get_decorated_name()));
    if (count == 0) {
      fpitem->setBackground(QColor(64,64,64));
    }
    _tptr->setItem(_threadName->row() + count, col, fpitem);
    ++count;
    runningKey += frame.get_decorated_name();
    _keys[runningKey] = col;
  }
}

void StackRow::add(const frenchroast::monitor::StackTrace& st)
{
  if (_keys.count(st.key()) != 0) {
    return;
  }

  std::string partialkey = "";
  for(auto& x : _complete_keys) {
    if (x.find(st.key()) != std::string::npos ) {  
      return;
    }
    if (st.key().find(x) != std::string::npos ) {
      partialkey = x;
    }
  }
  if (partialkey != "") {
    append_to_column(_keys[partialkey], st);
  }
  else {
    ++_col;
    if (_tptr->columnCount() < _col) {
      _tptr->insertColumn(_tptr->columnCount());
    }
    add_column(st, _col -1 );
  }
  _complete_keys.insert(st.key());
}
