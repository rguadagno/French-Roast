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

#include <iostream>
#include <QSplitter>
#include <QLabel>
#include <QHeaderView>
#include "MonitorUtil.h"
#include "SignalDelegate.h"
#include "TrafficViewer.h"
#include "QUtil.h"
#include "KeyListener.h"
#include "StackColumn.h"

namespace frenchroast {

  TrafficViewer* TrafficViewer::_instance{nullptr};
  const std::string TrafficViewer::FName{"traffic"};

  TrafficViewer::TrafficViewer(QWidget* parent) : FViewer(parent)
  {
    QObject::connect(_actionBar->add(new ActionButton("Start|Stop")), &ActionButton::request, 
                     this, &TrafficViewer::start_stop);
    
    _rate = _actionBar->add(new ActionEdit("100"));    
    _actionBar->add(new ActionLabel("sample rate (milliseconds):"));
    _traffic = new QTableWidget();
    _traffic->setStyleSheet(_settings->value("traffic_grid_style").toString());
    _traffic->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    _traffic->verticalHeader()->hide();
    _traffic->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    QWidget* trafficHolder = new QWidget();
    QGridLayout* tlayout = new QGridLayout();
    QLabel* label = new QLabel("Hot Stacks");
    label->setAlignment(Qt::AlignCenter);
    label->setStyleSheet("QLabel {background: #A0A0A0;color: black; font-family:\"Arial\";}");
    tlayout->addWidget(label, 0,0, 1, -1);
    tlayout->addWidget(_traffic, 1,0);
    tlayout->setRowStretch(1,10);
    tlayout->setSpacing(0);
    trafficHolder->setLayout(tlayout);
    trafficHolder->setStyleSheet(_settings->value("zero_border_style").toString());
    KeyListener* tkeyListener = new KeyListener;
    _traffic->installEventFilter(tkeyListener);
    QObject::connect(tkeyListener, &KeyListener::signalkey, this, [&](){if(_traffic->currentColumn() > 0) add_signal(_traffic->currentItem()->text());});

    _ranking = new QTableWidget();
    _ranking->insertColumn(0);
    _ranking->insertColumn(0);
    _ranking->horizontalHeader()->hide();
    _ranking->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    _ranking->verticalHeader()->hide();
    _ranking->setStyleSheet(_settings->value("traffic_grid_style").toString());
    QWidget* rankingHolder = new QWidget();
    QGridLayout* rlayout = new QGridLayout();
    QLabel* label2 = new QLabel("Hot Methods");
    label2->setAlignment(Qt::AlignCenter);
    label2->setStyleSheet("QLabel {background:#A0A0A0;color: black; font-family:\"Arial\";}");
    _ranking->setItemDelegateForColumn(0, new SignalDelegate(_ranking));
    _ranking->setItemDelegateForColumn(1, new SignalDelegate(_ranking));
    rlayout->addWidget(label2, 0,0, 1, -1);
    rlayout->addWidget(_ranking, 1,0);
    rlayout->setRowStretch(1,10);
    rlayout->setSpacing(0);
    rankingHolder->setLayout(rlayout);
    rankingHolder->setStyleSheet(_settings->value("zero_border_style").toString());
    KeyListener* keyListener = new KeyListener;
    _ranking->installEventFilter(keyListener);
    QObject::connect(keyListener, &KeyListener::signalkey, this, [&](){if(_ranking->currentColumn() == 1) add_signal(_ranking->currentItem()->text());});
    QSplitter* splitter = new QSplitter();
    splitter->setContentsMargins(0,0,0,0);
    splitter->addWidget(trafficHolder);
    splitter->addWidget(rankingHolder);
    splitter->setSizes({650,250});
    splitter->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setup_dockwin("Traffic", splitter, false);
  }

  
  void TrafficViewer::start_stop(const std::string& text)
  {
    if(text == "Start")
      start_watching(atoi(_rate->text().toStdString().c_str()));
    else
      stop_watching();
  }
  
  
  std::string getvalue(int v)
  {
    return v > 0 ? "X" : "";
  }
  
  void TrafficViewer::update_traffic(const std::vector<frenchroast::monitor::StackTrace>& stacks)
  {
    for(auto& x : stacks) {
      if(_thread_col.count(x.thread_name()) == 0  ) {
        _traffic->insertColumn(_traffic->columnCount());
        _traffic->setHorizontalHeaderItem(_traffic->columnCount() -1, createItem("monitors"));
                
        _traffic->insertColumn(_traffic->columnCount());
        _thread_col[x.thread_name()] = StackColumn{_traffic->columnCount()-1};
        _traffic->setHorizontalHeaderItem(_thread_col[x.thread_name()].column(), createItem(x.thread_name()));
        _traffic->setItemDelegateForColumn(_thread_col[x.thread_name()].column(), new SignalDelegate(_traffic));
      }

      if(_thread_col[x.thread_name()].update(x)) {
        int newrows = _thread_col[x.thread_name()].required_rows() - _traffic->rowCount();
        int totalrows = static_cast<int>(newrows +  _traffic->rowCount() + _thread_col[x.thread_name()].stacks().size());
        for(int r = _traffic->rowCount(); r < totalrows; r++) {
          _traffic->insertRow(r);
        }

        int col = _thread_col[x.thread_name()].column();
        
        int rowidx = 0;
        for(auto& trace : _thread_col[x.thread_name()].stacks()) {
          if(rowidx > 0 ) {
            if(_traffic->item(rowidx,col) != 0) {
              _traffic->item(rowidx,col)->setText("");
              _traffic->item(rowidx,col-1)->setText("");
            }
            ++rowidx;
          }
          for(auto& item : trace.descriptor_frames()) {
            if(_traffic->item(rowidx,col) == 0) {
              _traffic->setItem(rowidx,col,createItem(item));
            }
            else {
              _traffic->item(rowidx,col)->setText(QString::fromStdString(  item   ));
            }
            ++rowidx;
          }
        }
      
        
      rowidx = 0;
      for(auto& trace : _thread_col[x.thread_name()].stacks()) {
        if(rowidx > 0 ) {
          if(_traffic->item(rowidx,col-1) != 0) {
            _traffic->item(rowidx,col-1)->setText("");
          }
          ++rowidx;
        }
        for(auto& item : trace.monitor_frames()) {
          if(_traffic->item(rowidx,col-1) == 0) {
            _traffic->setItem(rowidx,col-1,createItem(getvalue(item)));
          }
          else {
            _traffic->item(rowidx,col-1)->setText(QString::fromStdString(   getvalue(item )   ));
          }
          ++rowidx;
        }
      }
      
      }
    }
  }


  void TrafficViewer::update_ranking(std::vector<frenchroast::monitor::MethodStats> ranks)
  {
    _ranking->clear();
    for(auto& x : ranks) {
      addRow(_ranking, createItem(x.invoked_count()), createItem(x.descriptor()));
    }
  }
  

  TrafficViewer* TrafficViewer::instance(QWidget* parent)
  {
    if(_instance != nullptr) return _instance;
    _instance = new TrafficViewer(parent);
    restore_win(FName, _settings, _instance, dynamic_cast<QMainWindow*>(parent));
    return _instance;
  }



  TrafficViewer::~TrafficViewer()
  {
    _instance = nullptr;
  }


  void TrafficViewer::capture()
  {
     capture_win(FName, _settings, _instance != nullptr ? _instance : nullptr);
  }

  bool TrafficViewer::restore_is_required()
  {
    return restore_required(FName, _settings);
  }

  void TrafficViewer::reset()
  {
    if(_instance == nullptr) return;
    _instance->reset_all();
  }

  void TrafficViewer::reset_all()
  {
    clearTable(_traffic, true);
    clearTable(_ranking);
    _traffic_keys.clear();
    _thread_col.clear(); 
  }

  bool TrafficViewer::up()
  {
      return _instance != nullptr;
  }

}
