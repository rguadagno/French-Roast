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

#include "ActionBar.h"
#include <QApplication>
#include <QPushButton>
#include <QStyle>

const std::bitset<4> ActionBar::None{"0000"};
const std::bitset<4> ActionBar::Close{"0001"};
const std::bitset<4> ActionBar::Save{"0010"};

ActionBar::ActionBar(const std::bitset<4>& actions)
  {
    _layout = new QGridLayout();
    QPushButton* exitbtn = nullptr;
    QPushButton* btn = nullptr;
    
    if((actions & Close) == Close) {
      QIcon closeicon = QApplication::style()->standardIcon(QStyle::SP_TitleBarCloseButton);
      exitbtn = new QPushButton(closeicon,"");
      _layout->addWidget(exitbtn, 0, 2);
      QObject::connect(exitbtn, &QPushButton::clicked, this, &ActionBar::close_clicked);
    }

    if((actions & Save) == Save) {
      btn = new QPushButton("save");
      btn->setStyleSheet("QPushButton {border: none; padding: 1px;font-size: 16px;background-color: #e2e9ff;}");
      _layout->addWidget(btn, 0, 0);
      QObject::connect(btn,     &QPushButton::clicked, this, &ActionBar::save_clicked);
    }

    setStyleSheet("QWidget {padding: 0px;}");
    _layout->setSpacing(0);
    _layout->setContentsMargins(0,0,10,0);
    setLayout(_layout);
  }

ActionBar::~ActionBar()
  {
    delete _layout;
  }
