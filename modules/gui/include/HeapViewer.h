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

#ifndef HEAPV_H
#define HEAPV_H


#include <QSettings>
#include <QTableWidget>
#include <QListWidget>
#include "FViewer.h"
#include "HeapReport.h"

namespace frenchroast {

  class HeapViewer : public FViewer {

    Q_OBJECT

  private:
    static HeapViewer*  _instance;
    static const std::string  FName;
    
    HeapViewer(QWidget*);
    ~HeapViewer();
    QTableWidget*   _data;
    std::unordered_map<std::string,QTableWidgetItem*> _classname;
    std::unordered_map<std::string,QTableWidgetItem*> _lifetime;
    std::unordered_map<std::string,QTableWidgetItem*> _current;
    std::unordered_map<std::string,QTableWidgetItem*> _max;

    
    void reset_all();    
    
  public:
    static HeapViewer* instance(QWidget*);
    static void capture();
    static void reset();
    static bool restore_is_required();
    static bool up();

    
  public slots:
    void update(const frenchroast::monitor::HeapReport& rpt);
      
  };


}

#endif
