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

#include "TrafficViewer.h"
#include <QSplitter>
#include <QLabel>
#include <QHeaderView>
#include "MonitorUtil.h"
#include <iostream>

namespace frenchroast {

  TrafficViewer* TrafficViewer::_instance{nullptr};

  TrafficViewer::TrafficViewer(QWidget* parent) : FViewer(parent)
  {

    _actionBar = new ActionBar(ActionBar::Close | ActionBar::StartStop);
    _traffic = new QTableWidget();
    _traffic->setStyleSheet(_settings->value("traffic_grid_style").toString());
    _traffic->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    _traffic->horizontalHeader()->hide();
    _traffic->verticalHeader()->hide();
    _traffic->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    _traffic->insertColumn(0);
  
    _ranking = new QListWidget();
    _ranking->setStyleSheet(_settings->value("list_style").toString());

    QWidget* trafficHolder = new QWidget();

    QGridLayout* tlayout = new QGridLayout();
    QLabel* label = new QLabel("Hot Stacks");
    label->setAlignment(Qt::AlignCenter);
    label->setStyleSheet("QLabel {background:green;color: white; font-family:\"Arial\";}");
    tlayout->addWidget(label, 0,0, 1, -1);
    tlayout->addWidget(_traffic, 1,0);
    tlayout->setRowStretch(1,10);
    tlayout->setSpacing(0);
    trafficHolder->setLayout(tlayout);
    trafficHolder->setStyleSheet(_settings->value("zero_border_style").toString());

    QWidget* rankingHolder = new QWidget();
    QGridLayout* rlayout = new QGridLayout();
    QLabel* label2 = new QLabel("Hot Methods");
    label2->setAlignment(Qt::AlignCenter);
    label2->setStyleSheet("QLabel {background:green;color: white; font-family:\"Arial\";}");
    rlayout->addWidget(label2, 0,0, 1, -1);
    rlayout->addWidget(_ranking, 1,0);
    rlayout->setRowStretch(1,10);
    rlayout->setSpacing(0);
    rankingHolder->setLayout(rlayout);
    rankingHolder->setStyleSheet(_settings->value("zero_border_style").toString());

    QSplitter* splitter = new QSplitter();
    splitter->setContentsMargins(0,0,0,0);
    splitter->addWidget(trafficHolder);

    splitter->addWidget(rankingHolder);
    splitter->setSizes({650,250});
    
    setup_dockwin("Traffic", splitter, false);
    QObject::connect(_actionBar, &ActionBar::start_clicked, this, &TrafficViewer::start_traffic);
    QObject::connect(_actionBar, &ActionBar::stop_clicked, this, &TrafficViewer::stop_traffic);
    
  }

  void TrafficViewer::start_traffic()
  {
    start_watching(10);
  }

  void TrafficViewer::stop_traffic()
  {
    stop_watching();
  }

  
  void TrafficViewer::update_traffic(const std::vector<frenchroast::monitor::StackTrace>& stacks)
  {
    for(auto& x : stacks) {
      int currRow = _traffic->rowCount();
      if(_traffic_rows.count(x.thread_name()) == 0) {
        _traffic->insertRow(currRow);
        StackRow* sr = new StackRow{x.thread_name(), currRow, _traffic, _traffic_keys};
        _traffic_rows[x.thread_name()] = sr;
      }
      _traffic_rows[x.thread_name()]->add(x);
    }
  }

  void TrafficViewer::update_ranking(std::vector<frenchroast::monitor::MethodStats> ranks)
  {

    _ranking->clear();
    for(auto& x : ranks) {
          std::cout << " * UPDATE RANKING *" << std::endl;
      std::string desc = std::to_string(x.invoked_count());
      frenchroast::monitor::pad(desc, 10);
      desc.append(x.descriptor());
      _ranking->addItem(new QListWidgetItem(QString::fromStdString(desc)));
    }
  }
  

  TrafficViewer* TrafficViewer::instance(QWidget* parent)
  {
    if(_instance != nullptr) return _instance;
    _instance = new TrafficViewer(parent);
    restore_win("traffic", _settings, _instance, dynamic_cast<QMainWindow*>(parent));
    return _instance;
  }



  TrafficViewer::~TrafficViewer()
  {
    _instance = nullptr;
  }


  void TrafficViewer::capture()
  {
     capture_win("traffic", _settings, _instance != nullptr ? _instance : nullptr);
  }




}
