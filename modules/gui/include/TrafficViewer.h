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

#ifndef TRAFFIC_H
#define TRAFFIC_H

#include <QSettings>
#include <QTableWidget>
#include <QListWidget>
#include <unordered_map>
#include "FViewer.h"
#include "StackColumn.h"
#include "StackTrace.h"
#include "MethodStats.h"



namespace frenchroast {

  class TrafficViewer : public FViewer {

    Q_OBJECT

  private:
    static TrafficViewer*  _instance;
    static const std::string  FName;
    TrafficViewer(QWidget*);
    ~TrafficViewer();

    QTableWidget*                             _traffic;
    QTableWidget*                             _ranking;
    std::unordered_map<std::string, int>      _traffic_keys;
    ActionEdit*                               _rate;

    std::unordered_map<std::string, StackColumn>      _thread_col;
    
    
  public:
    static TrafficViewer* instance(QWidget*);
    static void capture();
    static bool restore_is_required();
    
  private slots:
    void start_stop(const std::string& );
    
  public slots:
      void update_traffic(const std::vector<frenchroast::monitor::StackTrace>& stacks);
      void update_ranking(std::vector<frenchroast::monitor::MethodStats> ranks);
      
  signals:
    void start_watching(int rate);
    void stop_watching();
    void add_signal(QString);
    
  };


}

#endif
