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

#ifndef DETAILVIEWER_H
#define DETAILVIEWER_H
#include <QTableWidget>
#include <QTabWidget>
#include <QSettings>
#include "DetailHolder.h"



class DetailViewer : public QTabWidget {

  Q_OBJECT
  
  std::string                                        _descriptor;
  QTableWidget*                                      _argData;
  QTableWidget*                                      _stackData;
  std::unordered_map<std::string, QTableWidgetItem*> _items;
  std::unordered_map<std::string, int>               _detailItems;
 public:
  DetailViewer(const std::string& descriptor, QSettings&);

  public slots:
    
   void update(const std::string& descriptor, const DetailHolder& holder);




};

#endif
