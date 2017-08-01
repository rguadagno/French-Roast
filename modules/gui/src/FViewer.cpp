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

#include "FViewer.h"
#include "SignalDelegate.h"
#include <QListWidget>
#include <QMenu>
#include <QApplication>
#include <iostream>

namespace frenchroast {

  FViewer::FViewer(QWidget* parent) : _parent(parent)
  {
    _actionBar = new ActionBar();
  }
  
  void FViewer::setup_dockwin(const std::string& title, QWidget* wptr, bool codeMode)
  {
    _dock = new QDockWidget(QString::fromStdString(title), _parent);
  _dock->setStyleSheet(_settings->value("dock_widget_style").toString());
  QListWidget* sigLabel = new QListWidget{};
  QWidget*     titlebar = new QWidget();
  QGridLayout* layout = new QGridLayout();

  _title  = new QListWidgetItem{QString::fromStdString(title)};
  _title->setSizeHint(QSize(30,30)); 
  sigLabel->setEnabled(false);
  sigLabel->setFixedHeight(28);
  sigLabel->addItem(_title);
  sigLabel->setStyleSheet(_settings->value("dock_title_style2").toString());
  if(codeMode) {
    sigLabel->setItemDelegate(new SignalDelegate(sigLabel));
  }

  QPushButton* fbmenu = new QPushButton("French\nRoast");
  fbmenu->setStyleSheet("QPushButton {background: #404040;color:#521704; "\
                        "border: 1px solid #303030;" \
                        "font-size:12px;" \
                        "border-top-left-radius:6px;"               \
                        "border-top-right-radius:6px;"                 \
                        "border-bottom-right-radius:6px;"              \
                        "border-bottom-left-radius:6px; }"          \
                        "QPushButton::hover{background-color:#202020;}"
                        );
  QMenu* menu = new QMenu();
  fbmenu->setMenu(menu);

  QObject::connect(menu->addAction("Signals"),               &QAction::triggered, this, &FViewer::signal_viewer);
  QObject::connect(menu->addAction("Timers"),                &QAction::triggered, this, &FViewer::timer_viewer);
  QObject::connect(menu->addAction("Editor"),                &QAction::triggered, this, &FViewer::editor_viewer);
  QObject::connect(menu->addAction("Traffic watcher"),       &QAction::triggered, this, &FViewer::traffic_viewer);
  QObject::connect(menu->addAction("Class loading watcher"), &QAction::triggered, this, &FViewer::classload_viewer);
  QObject::connect(menu->addAction("About | Help"),          &QAction::triggered, this, &FViewer::about_viewer);
  QObject::connect(menu->addAction("Exit French-Roast"),     &QAction::triggered, this, &FViewer::exit_fr);

  layout->addWidget(fbmenu,1,1);
  layout->addWidget(sigLabel,1,3);
  layout->setContentsMargins(1,1,1,1);
  layout->addWidget(_actionBar,1, 5);
  titlebar->setLayout(layout);
  titlebar->setStyleSheet(_settings->value("dock_win_header_style").toString());

  _dock->setTitleBarWidget(titlebar);
  _dock->setAttribute(Qt::WA_DeleteOnClose);
  _dock->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
  _dock->setWidget(wptr);
  _dock->setFloating(true);
  QObject::connect(_actionBar, &ActionBar::close_clicked, _dock, &QDockWidget::close);
  QObject::connect(_dock, &QDockWidget::destroyed, this, [&](){delete this;});
  QObject::connect(_actionBar, &ActionBar::close_clicked, this, [&](){closed();});
}

  
  FViewer::operator QDockWidget*()
  {
    return _dock;
  }

  void FViewer::setSettings(QSettings* settings)
  {
    _settings = settings;
  }


  void FViewer::update_title(const std::string& title) {
    _title->setText(QString::fromStdString(title));
  }
  
  void FViewer::resize_win(int width, int height) {
    _dock->resize(width, height);
  }

  void FViewer::move(int x, int y) {
    _dock->move(x,y);
  }
  
  void capture_win(const std::string& name, QSettings* settings, FViewer* view)
  {
    if(view != nullptr) {
      QDockWidget* win = *view;
      settings->setValue(QString::fromStdString(name  + ":up"),    true);
      settings->setValue(QString::fromStdString(name  + ":width"),  win->width());
      settings->setValue(QString::fromStdString(name  + ":height"), win->height());
      settings->setValue(QString::fromStdString(name  + ":xpos"),   win->pos().x());
      settings->setValue(QString::fromStdString(name  + ":ypos"),   win->pos().y());
    }
    else {
      settings->setValue(QString::fromStdString(name + ":up"),    false);
    }
  }

  void restore_win(const std::string& name, QSettings* settings, FViewer* view, QMainWindow* mainwin)
  {
    QDockWidget* win = *view;
    settings->setValue(QString::fromStdString(name + ":up"), true);
    win->resize(settings->value(QString::fromStdString(name + ":width"), mainwin->width()).toInt(),
                           settings->value(QString::fromStdString(name + ":height"), 200).toInt()
                           );

    win->move(settings->value(QString::fromStdString(name + ":xpos"), mainwin->width() /2).toInt(),
              settings->value(QString::fromStdString(name + ":ypos"), mainwin->height() /2).toInt());
  mainwin->addDockWidget(Qt::TopDockWidgetArea,    win);
    
  }


  
}
