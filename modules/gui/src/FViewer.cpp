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


namespace frenchroast {
  FViewer::FViewer(QSettings& settings, QWidget* parent) : _settings(settings), _parent(parent)
  {
  }
  
  void FViewer::setup_dockwin(const std::string& title, QWidget* wptr, bool codeMode)
  {
    _dock = new QDockWidget(QString::fromStdString(title), _parent);
  _dock->setStyleSheet(_settings.value("dock_widget_style").toString());
  QListWidget* sigLabel = new QListWidget{};
  QWidget*     titlebar = new QWidget();
  QGridLayout* layout = new QGridLayout();

  
  QListWidgetItem* item = new QListWidgetItem{QString::fromStdString(title)};
  item->setSizeHint(QSize(30,24));
  sigLabel->setEnabled(false);
  sigLabel->setFixedHeight(22);
  sigLabel->addItem(item);
  sigLabel->setStyleSheet(_settings.value("dock_title_style2").toString());
  if(codeMode) {
    sigLabel->setItemDelegate(new SignalDelegate(sigLabel));
  }

  layout->addWidget(sigLabel,1,1);
  layout->setContentsMargins(1,1,1,1);
  layout->addWidget(_actionBar,1, 5);
  titlebar->setLayout(layout);
  titlebar->setStyleSheet(_settings.value("dock_win_header_style").toString());

  _dock->setTitleBarWidget(titlebar);
  _dock->setAttribute(Qt::WA_DeleteOnClose);
  _dock->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
  _dock->setWidget(wptr);
  _dock->setFloating(true);
}

  FViewer::operator QDockWidget*()
  {
    return _dock;
  }
}
