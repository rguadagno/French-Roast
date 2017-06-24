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

#include <QTWidgets/QApplication>
#include <QTWidgets/QLabel>
#include <QTWidgets/QHBoxLayout>
#include <QTWidgets/QListWidget>
#include <QTWidgets/QMessageBox>
#include <QTWidgets/QStatusBar>
#include <QTWidgets/QSizePolicy>
#include <QTWidgets/QPushButton>
#include <QTWidgets/QTableWidget>
#include <QTWidgets/QDockWidget>
#include <QTCore/QObject>
#include <QtGlobal>
#include <QThread>
#include <QHeaderView>
#include <QAction>
#include <QFont>
#include <string>
#include "fr.h"
#include "FRMain.h"
#include "MonitorUtil.h"
#include "StackRow.h"


int main(int argc, char* argv[]) {
  qRegisterMetaType<std::string>();
  qRegisterMetaType<std::vector<frenchroast::monitor::StackTrace>>();
  qRegisterMetaType<std::vector<frenchroast::monitor::MarkerField>>();
  
  QApplication app(argc,argv);

  if (argc != 3) {
    QMessageBox box;
    box.setText("usage: roaster IP PORT\nexample: roaster 127.0.0.1 6060");
    box.exec();
    exit(0);
  }

  FRListener roaster{std::string{argv[1]}, atoi(argv[2])};
  QThread* tt = new QThread(&roaster);

  roaster.moveToThread(tt);
  
  FRMain main(&roaster);

  QObject::connect(&roaster, &FRListener::thooked,        &main,    &FRMain::update_list);
  QObject::connect(&roaster, &FRListener::timersignal,    &main,    &FRMain::update_timed_list);
  QObject::connect(&roaster, &FRListener::traffic_signal, &main,    &FRMain::update_traffic);
  QObject::connect(tt,       &QThread::started,           &roaster, &FRListener::init);
  QObject::connect(&app,     &QApplication::aboutToQuit,  &main,    &FRMain::handle_exit);

  tt->start();
  main.setWindowTitle("French Roast");
  main.show();

 return app.exec();
}

class SignalItem : public QListWidgetItem {
  QString _text;
  static QFont _font;
public:
  SignalItem(const QString& text, int total) : _text(text), QListWidgetItem( text + "  " + QString::fromStdString(frenchroast::monitor::ntoa(total)))
  {
    setFont(_font);
  }

  const QString& gettext() const
  {
    return _text;
  }

};


class CodeFont : public QFont {
public:
  CodeFont() : QFont(QString::fromStdString("Inconsolata"), 12, QFont::Normal)
  {
    setStyleHint(QFont::TypeWriter);
  }
};

QFont SignalItem::_font = CodeFont();

FRMain::FRMain(FRListener* listener)
{
  resize(1300,300);
  _list               = new QListWidget;
  _timedlist          = new QListWidget;
  _traffic            = new QTableWidget;
  _buttonStartTraffic = new QPushButton{"Start"};
  _buttonStopTraffic  = new QPushButton{"Stop"};
  _rate               = new QLineEdit;

  addDockWidget(Qt::TopDockWidgetArea,    setup_list("Signals", _list, QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable));
  addDockWidget(Qt::TopDockWidgetArea,    setup_list("Timers", _timedlist, QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable));
  addDockWidget(Qt::BottomDockWidgetArea, build_traffic_viewer(_traffic,_buttonStartTraffic,_buttonStopTraffic,_rate));
  
  _traffic->insertColumn(0);
  
  QObject::connect(_traffic,            &QTableWidget::itemDoubleClicked, this,     &FRMain::show_deco);
  QObject::connect(_list,               &QListWidget::itemDoubleClicked,  this,     &FRMain::show_detail);
  QObject::connect(_buttonStartTraffic, &QPushButton::clicked,            this,     &FRMain::update_traffic_rate);
  QObject::connect(_buttonStopTraffic,  &QPushButton::clicked,            listener, &FRListener::stop_traffic);
  QObject::connect(this,                &FRMain::start_traffic,           listener, &FRListener::start_traffic);
  QObject::connect(listener,            &FRListener::remoteconnected,     this,     &FRMain::update_status);
  QObject::connect(listener,            &FRListener::remoteunloaded,      this,     &FRMain::update_unloaded_status);
  
  statusBar()->showMessage("waiting for connection...");
}


QDockWidget* FRMain::setup_list(const std::string title, QListWidget* list_ptr, QDockWidget::DockWidgetFeatures features)
{
  list_ptr->setStyleSheet("QListWidget {border: 1px solid grey;background-color: black;font-size: 16px;font-family: \"Arial\"} QListWidget::item {color: orange;}");

  QDockWidget* holder = new QDockWidget(QString::fromStdString(title), this);
  holder->setStyleSheet("QWidget {border: 0px solid grey;background-color: grey;font-size:16px;font-family: \"Arial\"}");


  QLabel* sigLabel = new QLabel(QString::fromStdString(title));
  QWidget* titlebar = new QWidget();
  QBoxLayout* layout = new QBoxLayout(QBoxLayout::LeftToRight);
  layout->addWidget(sigLabel);
  if((features & QDockWidget::DockWidgetClosable) == QDockWidget::DockWidgetClosable) {
    QPushButton* btn = new QPushButton();
    QObject::connect(btn, &QPushButton::clicked, holder, &QDockWidget::close);
    btn->setIcon(style()->standardIcon(QStyle::SP_DockWidgetCloseButton));
    layout->addWidget(btn,1, Qt::AlignRight);
  }
  titlebar->setLayout(layout);
  sigLabel->setStyleSheet("QLabel {border: 1px solid grey;qproperty-alignment: AlignHCenter;color:#bababa;foreground-color:white;background-color: grey;font-size:16px;font-family: \"Arial\"}");
  holder->setTitleBarWidget(titlebar);
  holder->setAttribute(Qt::WA_DeleteOnClose);
  holder->setFeatures(features);
  QVBoxLayout* vlayout = new QVBoxLayout();
  holder->setLayout(vlayout);
  holder->setWidget(list_ptr);

  return holder;
}



QDockWidget* FRMain::build_traffic_viewer(QTableWidget* grid, QPushButton* bstart, QPushButton* bstop, QLineEdit* rate)
{
  QWidget* buttonHolder = new QWidget;
  QHBoxLayout* hlayout = new QHBoxLayout();
  QLabel* desc = new QLabel{QString::fromStdString("rate (millisec):")};
  desc->setAlignment(Qt::AlignRight);
  desc->setStyleSheet("QLabel {font-size:16px;font-family: \"Arial\"}");
  
  buttonHolder->setLayout(hlayout);
  rate->setText("100");
  bstart->setSizePolicy({QSizePolicy::Fixed,QSizePolicy::Fixed});
  bstop->setSizePolicy({QSizePolicy::Fixed,QSizePolicy::Fixed});
  rate->setSizePolicy({QSizePolicy::Fixed,QSizePolicy::Fixed});

  hlayout->addWidget(desc,0,Qt::AlignLeft);
  hlayout->addWidget(rate,0,Qt::AlignLeft);
  hlayout->addWidget(bstart,0,Qt::AlignLeft);
  
  buttonHolder->setStyleSheet("QWidget {border: 1px solid grey;background-color: grey;font-size: 16px;font-family: \"Arial\"} ");
  rate->setStyleSheet("QLineEdit {padding: 2px; border: 1px solid black;background-color: #C9C9C9;font-size: 16px;font-family: \"Arial\"}  ");

  rate->setFixedWidth(50);
  rate->setInputMask("0000");
  rate->setAlignment(Qt::AlignRight);

    bstart->setStyleSheet("QPushButton {padding: 2px; border: 1px solid black;background-color: #CF78CA;font-size: 16px;font-family: \"Arial\"} QPushButton::hover:!pressed {font-size: 20px;} ");
    bstart->setFixedWidth(70);


    hlayout->addWidget(bstop,1,Qt::AlignLeft);
    bstop->setStyleSheet("QPushButton {padding: 2px; border: 1px solid black;background-color: #8F6A82;font-size: 16px;font-family: \"Arial\"} QPushButton::hover:!pressed {font-size: 20px;} ");
    bstop->setFixedWidth(70);

    QDockWidget* docwin = new QDockWidget(QString::fromStdString("Traffic"), this);
    QWidget* holder = new QWidget();

    docwin->setWidget(holder);

    QVBoxLayout* vlayout = new QVBoxLayout();
    vlayout->setSpacing(0);
    holder->setLayout(vlayout);

   
  grid->setStyleSheet("QTableWidget {border: 0px solid grey;background-color: black;font-size: 16px;font-family: \"Arial\"} QTableWidget::item {color: orange;}");

  grid->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
  grid->horizontalHeader()->hide();
  grid->verticalHeader()->hide();
  grid->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  
  vlayout->addWidget(grid);

  QLabel* sigLabel = new QLabel(QString::fromStdString("Traffic"));
  QWidget* titlebar = new QWidget();
  QBoxLayout* layout = new QBoxLayout(QBoxLayout::LeftToRight);
  layout->setSpacing(0);
  layout->addWidget(buttonHolder,1,Qt::AlignLeft);
  layout->addWidget(sigLabel,1, Qt::AlignLeft);
  
  titlebar->setLayout(layout);
  sigLabel->setStyleSheet("QLabel {border: 1px solid grey;qproperty-alignment: AlignHCenter;color:#bababa;background-color: grey;font-size:16px;font-family: \"Arial\"}");
  docwin->setTitleBarWidget(titlebar);

  holder->setStyleSheet("QWidget {border: 0px solid grey;background-color: black;font-size:16px;font-family: \"Arial\"}");
  docwin->setStyleSheet("QWidget {border: 1px solid grey;background-color: grey;font-size:16px;font-family: \"Arial\"}");

  return docwin;
}


QWidget* setup_central(QWidget* lists)
{
  QWidget* holder = new QWidget;
  holder->setStyleSheet("QWidget {background-color: black;}");
  QVBoxLayout* vlayout = new QVBoxLayout();
  vlayout->setSpacing(0);
  holder->setLayout(vlayout);
  vlayout->addWidget(lists);
  return holder;
}


void FunctionPoint::set_decorated_name(const QString& name)
{
  _name = name;
}

QString FunctionPoint::get_name() const
{
  return _name;
}


void FRMain::show_deco(QTableWidgetItem* item)
{
  QMessageBox box;
  FunctionPoint* ptr = dynamic_cast<FunctionPoint*>(item);
  box.setText(ptr->get_name());
  box.exec();
}

void FRMain::show_detail(QListWidgetItem* item)
{
  SignalItem* ptr = dynamic_cast<SignalItem*>(item);
  std::string sname{ptr->gettext().toStdString()};
  if(_detailLists.count(sname) > 0 || sname.find("[M]") == std::string::npos) {
    return;
  }
  _detailLists[sname] = new QListWidget(); 
  _detailLists[sname]->setObjectName(QString::fromStdString(sname));
  QObject::connect(_detailLists[sname], &QListWidget::destroyed, this, &FRMain::destroy_list);  
  addDockWidget(Qt::TopDockWidgetArea, setup_list(sname, _detailLists[sname]));
  update_detail_list(_detailLists[ sname ], _detailDescriptors[sname]);

}

void FRMain::destroy_list(QObject* obj)
{
  if(_exit) return;
  std::string name = obj->objectName().toStdString();
  if(_detailItemsPerList.count(name) == 1) {
    for(auto& x : _detailItemsPerList[name]) {
      _detailItems.erase(x);
    }
    _detailItemsPerList.erase(name);
  }
  _detailLists.erase(name);
  
}


void FRMain::update_traffic_rate()
{
  start_traffic(atoi(_rate->text().toStdString().c_str()));
}



void FRMain::update_status(std::string msg)
{
  statusBar()->showMessage(QString::fromStdString(msg));
}

void FRMain::update_unloaded_status(std::string msg)
{
  statusBar()->setStyleSheet("QWidget {background-color: #ab1e1e;}");
  statusBar()->showMessage(QString::fromStdString(msg));
}


void FRMain::update_list(std::string ltype, std::string  descriptor, int count, const std::vector<frenchroast::monitor::MarkerField>& markers)
{
  if(markers.size() > 0) {
    descriptor = "[M] " + descriptor;
  }
  else {
 descriptor = "    " + descriptor;
  }
  
  _detailDescriptors[descriptor] = markers;
  if (_descriptors.count(descriptor) == 0 ) {
    _descriptors[descriptor] = new SignalItem(QString::fromStdString(descriptor), count);
    _list->addItem(_descriptors[descriptor]);
  }
  else {
    _descriptors[descriptor]->setText(QString::fromStdString(descriptor) + "  " + QString::fromStdString(frenchroast::monitor::ntoa(count)));
      if(_detailLists.count(descriptor) == 1) { 
        update_detail_list(_detailLists[descriptor], markers);
      }
  }
}

void FRMain::update_detail_list(QListWidget* list, const std::vector<frenchroast::monitor::MarkerField>& markers)
{
  for(auto& item : markers) {
    if(_detailItems.count(item._descriptor) == 0) {
      _detailItems[item._descriptor] = new QListWidgetItem(QString::fromStdString(item._descriptor) + "  " + QString::fromStdString(frenchroast::monitor::ntoa(item._count)));
      list->addItem(_detailItems[item._descriptor]);
      _detailItemsPerList[ list->objectName().toStdString() ].push_back(item._descriptor);
    }
    else {
      _detailItems[item._descriptor]->setText(QString::fromStdString(item._descriptor) + "  " + QString::fromStdString(frenchroast::monitor::ntoa(item._count)));      
    }
  }
}

void FRMain::update_traffic(const std::vector<frenchroast::monitor::StackTrace>& stacks)
{
  for(auto& x : stacks) {
    if (_traffic_keys.count(x.key()) != 0) {
      continue;
    }
    int currRow = _traffic->rowCount();
    if(_traffic_rows.count(x.thread_name()) == 0) {
      _traffic->insertRow(currRow);
      StackRow* sr = new StackRow{x.thread_name(), currRow, _traffic, _traffic_keys};
      _traffic_rows[x.thread_name()] = sr;
    }
    _traffic_rows[x.thread_name()]->add(x);
  }
}



void FRMain::update_timed_list(std::string  descriptor, long elapsed)
{
  if (_descriptors.count(descriptor) == 0 ) {
    _descriptors[descriptor] = new QListWidgetItem(QString::fromStdString(descriptor) + "  " + QString::fromStdString(frenchroast::monitor::format_millis(elapsed)));
    _timedlist->addItem(_descriptors[descriptor]);
  }
  else
    _descriptors[descriptor]->setText(QString::fromStdString(descriptor) + "  " + QString::fromStdString(frenchroast::monitor::format_millis(elapsed)));
}

void FRMain::handle_exit()
{
  _exit = true;
}





