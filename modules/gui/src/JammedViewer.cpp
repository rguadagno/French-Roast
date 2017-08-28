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



#include <QHeaderView>
#include <QVBoxLayout>
#include "QUtil.h"
#include "SignalDelegate.h"
#include "KeyListener.h"
#include "JammedViewer.h"
#include "Util.h"


namespace frenchroast {
  
  JammedViewer::JammedViewer(QWidget* parent) : FViewer( parent)
  {
    _data = new QTableWidget;
    _data->setStyleSheet(_settings->value("traffic_grid_style").toString());
    _data->verticalHeader()->hide();
    _data->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    QVBoxLayout* vlayout = new QVBoxLayout();
    vlayout->setSpacing(0);
    vlayout->addWidget(_data );
    vlayout->setContentsMargins(0,0,0,0);
    setLayout(vlayout);
    setStyleSheet(_settings->value("zero_border_style").toString());
    
    _data->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);    
    _data->insertColumn(0);
    _data->setHorizontalHeaderItem(0, createItem("Count"));
    _data->setItemDelegateForColumn(0, new SignalDelegate(_data)); 
    
    _data->insertColumn(1);
    _data->setHorizontalHeaderItem(1, createItem("Monitors"));
    _data->setItemDelegateForColumn(1, new SignalDelegate(_data)); 
    
    _data->insertColumn(2);
    _data->setHorizontalHeaderItem(2, createItem("Waiter"));
    _data->setItemDelegateForColumn(2, new SignalDelegate(_data)); 

    _data->insertColumn(3);
    _data->setHorizontalHeaderItem(3, createItem("Owner"));
    _data->setItemDelegateForColumn(3, new SignalDelegate(_data)); 
    
    setMinimumSize(700,350);
    KeyListener* keyListener = new KeyListener;
    _data->installEventFilter(keyListener);
    setup_dockwin("Jammed Stacks", _data, false);
    QObject::connect(keyListener  , &KeyListener::signalkey,          this, [&](){handle_add_signal(_data->currentColumn()); });
}

void JammedViewer::update(const frenchroast::monitor::JammedReport& rpt)
{
  if(_jamsRow.count(rpt.key()) == 0) {
    if(_data->rowCount() > 0) {
      _data->insertRow(_data->rowCount());
    }
    _jamsRow[rpt.key()] = _data->rowCount();
    int newrows = rpt.waiter().size() > rpt.owner().size() ? rpt.waiter().size() : rpt.owner().size();
    for(int idx = 0; idx < newrows;idx++) {
      _data->insertRow(_data->rowCount());
    }

    int start = _jamsRow[rpt.key()];
    for(auto& x : rpt.waiter().descriptor_frames()) {
      _data->setItem(start++,COL_WAITER, createItem(x));
    }

    start = _jamsRow[rpt.key()];
    for(auto& x : rpt.owner().descriptor_frames()) {
      _data->setItem(start++,COL_OWNER, createItem(x));
    }

    start = _jamsRow[rpt.key()];
    for(auto& x : rpt.monitors()) {
      _data->setItem(start++,COL_MONITORS, createItem(x));
    }
  }

  _data->setItem(_jamsRow[rpt.key()], COL_COUNT, createItem(++_jamsCount[rpt.key()]));
}


  void JammedViewer::handle_add_signal(int col )
  {
    if(col == COL_COUNT || col == COL_MONITORS) return;
    if(_data->currentItem() == 0) return;
    std::string name = _data->currentItem()->text().toStdString();
    frenchroast::remove_blanks(name);
    add_signal(QString::fromStdString(name));
  }

  JammedViewer* JammedViewer::_instance{nullptr};
  const std::string JammedViewer::FName{"jammedviewer"};

  
  JammedViewer::~JammedViewer()
  {
    _instance = nullptr;
  }

  JammedViewer* JammedViewer::instance(QWidget* parent)
  {
    if(_instance != nullptr) return _instance;
    _instance = new JammedViewer(parent);
    restore_win(FName, _settings, _instance, dynamic_cast<QMainWindow*>(parent));
    return _instance;
  }



  void JammedViewer::capture()
  {
     capture_win(FName, _settings, _instance != nullptr ? _instance : nullptr);
  }

  void JammedViewer::reset()
  {
    if(_instance == nullptr) return;
    _instance->reset_all();
  }
  void JammedViewer::reset_all()
  {
    clearTable(_data);
    _jamsRow.clear();
    _jamsCount.clear();
  }
  
  bool JammedViewer::restore_is_required()
  {
    return restore_required(FName, _settings);
  }


  bool JammedViewer::up()
  {
      return _instance != nullptr;
  }

}
