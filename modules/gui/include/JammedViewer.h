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

#ifndef JAMMEDV_H
#define JAMMEDV_H

#include <QSettings>
#include <QTableWidget>
#include <QListWidget>
#include <unordered_map>
#include "FViewer.h"
#include "JammedReport.h"


namespace frenchroast {

  class JammedViewer : public FViewer {

    Q_OBJECT

  private:
    static JammedViewer*  _instance;
    static const std::string  FName;
    const int    COL_COUNT = 0;
    const int    COL_MONITORS = 1;
    const int    COL_WAITER = 2;
    const int    COL_OWNER = 3;
    
    JammedViewer(QWidget*);
    ~JammedViewer();
    void handle_add_signal(int row, int col);
    QTableWidget*                             _data;
    std::unordered_map<std::string, int>      _jamsRow;
    std::unordered_map<std::string, int>      _jamsCount;
    
    
  public:
    static JammedViewer* instance(QWidget*);
    static void capture();
    static bool restore_is_required();
    
  public slots:
    void update(const frenchroast::monitor::JammedReport& rpt);
      
  signals:
    void add_signal(QString);
    
  };


}

#endif
