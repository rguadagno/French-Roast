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
#include <unordered_map>
#include "fr.h"


class FRMain : public QMainWindow {
  Q_OBJECT

 public:
  FRMain(FRListener*);

 private:
  QListWidget* _list;
  QListWidget* _timedlist;
  std::unordered_map<std::string,QListWidgetItem*> _descriptors;
 
 public slots:
   void update_list(std::string, std::string,int);
   void FRMain::update_timed_list(std::string ltype, std::string  descriptor, long elapsed, int last);
   void update_status(std::string);
};

#endif
