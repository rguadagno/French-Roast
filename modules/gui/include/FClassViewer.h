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

#ifndef FCLASSVIEWER_H
#define FCLASSVIEWER_H
#include <QWidget>
#include <QSettings>
#include <QTableWidget>
#include "FViewer.h"
#include <unordered_map>
#include "FListItem.h"
#include "ClassDetail.h"

namespace frenchroast {
  class FClassViewer : public FViewer {


    
  QTableWidget*                                             _data;
  std::unordered_map<std::string, std::vector<std::string>> _methods;
  std::unordered_map<std::string, int>                      _ind;
  static FClassViewer*                                      _instance;
  static const std::string                                  Name;
  
  FClassViewer(QWidget*);
  ~FClassViewer();
  void expand_methods(const std::string&, int);
  void collapse_methods(const std::string&, int);
  
Q_OBJECT
 
 public:
 static FClassViewer* instance(QWidget*);
 static void capture();
 
 signals:
  void add_signal(QString);
  void start_watching();
  void stop_watching();
  
 public slots:
    
    void update(const std::vector<frenchroast::monitor::ClassDetail>&);
    void methods_for_class(QTableWidgetItem*);

 private slots:
   void handle_add_signal(int, QString);

  };
}
#endif
