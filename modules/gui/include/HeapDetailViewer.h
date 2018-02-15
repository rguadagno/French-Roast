// copyright (c) 2018 Richard Guadagno
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

#ifndef HEAPDETAILVIEWER_H
#define HEAPDETAILVIEWER_H

#include <QTableWidget>
#include <QListWidget>
#include <QSettings>
#include "FViewer.h"
#include "HeapReport.h"

namespace frenchroast {

class HeapDetailViewer : public FViewer {

  Q_OBJECT
  
  std::string                                        _class_name;
  QTableWidget*                                      _data;
  std::unordered_map<std::string, QTableWidgetItem*> _lifetime_widgets;
  std::unordered_map<std::string, QTableWidgetItem*> _stack_widgets;

  HeapDetailViewer(QWidget*, const std::string& descriptor);
  
 public:
  
  static HeapDetailViewer* instance(QWidget*, const std::string&);
  
  public slots:
    
    void update(const std::string& class_name,  frenchroast::monitor::HeapReport* rpt);
    
 signals:
    void add_signal(QString);


};
}
#endif
