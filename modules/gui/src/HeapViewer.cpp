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



#include <QHeaderView>
#include <QVBoxLayout>
#include "QUtil.h"
#include "SignalDelegate.h"
#include "HeapViewer.h"
#include "Util.h"
#include "MonitorUtil.h"


namespace frenchroast {
  
  HeapViewer::HeapViewer(QWidget* parent) : FViewer( parent), _data(new QTableWidget)
  {
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
    _data->setHorizontalHeaderItem(0, createItem("Class"));
    _data->setItemDelegateForColumn(0, new SignalDelegate(_data)); 
    
    _data->insertColumn(1);
    _data->setHorizontalHeaderItem(1, createItem("Lifetime"));
    _data->setItemDelegateForColumn(1, new SignalDelegate(_data)); 
    
    _data->insertColumn(2);
    _data->setHorizontalHeaderItem(2, createItem("Current"));
    _data->setItemDelegateForColumn(2, new SignalDelegate(_data)); 

    _data->insertColumn(3);
    _data->setHorizontalHeaderItem(3, createItem("Max"));
    _data->setItemDelegateForColumn(3, new SignalDelegate(_data)); 

    _data->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    _data->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    _data->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    _data->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);

    setMinimumSize(1500,450);
    setup_dockwin("Heap Monitor", _data, false);
    QObject::connect(_data,  &QTableWidget::itemDoubleClicked, this, [&](QTableWidgetItem* item){if(_data->currentColumn() == 0) view_detail_request(item->text().toStdString());});
}

  void HeapViewer::update(const frenchroast::monitor::HeapReport& rpt)
  {

    if (_classname.count(rpt.classname()) == 0 ) {
      _classname[rpt.classname()] = createItem(rpt.classname());
      _lifetime[rpt.classname()] = createItem(rpt.lifetime());
      _current[rpt.classname()] = createItem(rpt.current());
      _max[rpt.classname()] = createItem(rpt.max());

      addRow(_data, _classname[rpt.classname()], _lifetime[rpt.classname()], _current[rpt.classname()],_max[rpt.classname()]);
    }
    else {
      _lifetime[rpt.classname()]->setText(QString::fromStdString(frenchroast::monitor::ntoa(rpt.lifetime(),5, ' ')));
      _current[rpt.classname()]->setText(QString::fromStdString(frenchroast::monitor::ntoa(rpt.current(),5, ' ')));
      _max[rpt.classname()]->setText(QString::fromStdString(frenchroast::monitor::ntoa(rpt.max(),5, ' ')));

    }
  }

  HeapViewer* HeapViewer::_instance{nullptr};
  const std::string HeapViewer::FName{"heapviewer"};

  
  HeapViewer::~HeapViewer()
  {
    _instance = nullptr;
  }

  HeapViewer* HeapViewer::instance(QWidget* parent)
  {
    if(_instance != nullptr) return _instance;
    _instance = new HeapViewer(parent);
    restore_win(FName, _settings, _instance, dynamic_cast<QMainWindow*>(parent));
    return _instance;
  }



  void HeapViewer::capture()
  {
     capture_win(FName, _settings, _instance != nullptr ? _instance : nullptr);
  }

  void HeapViewer::reset()
  {
    if(_instance == nullptr) return;
    _instance->reset_all();
  }
  void HeapViewer::reset_all()
  {
    clearTable(_data);
    _classname.clear();
    _current.clear();
    _max.clear();
    _lifetime.clear();
  }
  
  bool HeapViewer::restore_is_required()
  {
    return restore_required(FName, _settings);
  }


  bool HeapViewer::up()
  {
      return _instance != nullptr;
  }

}
