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
#include <QLabel>

const std::bitset<4> ActionBar::None{"0000"};
const std::bitset<4> ActionBar::Close{"0001"};
const std::bitset<4> ActionBar::Save{"0010"};
const std::bitset<4> ActionBar::Validate{"0100"};
const std::bitset<4> ActionBar::StartStop{"1000"};

const QString ActionBar::START_TEXT{" start "};
const QString ActionBar::STOP_TEXT{" stop "};


ActionBar::ActionBar(const std::bitset<4>& actions)
  {
    _layout = new QGridLayout();
    
    if((actions & Close) == Close) {
      QIcon closeicon = QApplication::style()->standardIcon(QStyle::SP_TitleBarCloseButton);
      _closeButton = new QPushButton("X");
      _closeButton->setFixedSize(20,20);
      _closeButton->setStyleSheet(
                                  "QPushButton {color:black;" \
                                  "border: 1px solid black;" \
                                  "border-top-left-radius:3px;" \
                                  "border-top-right-radius:3px;" \
                                  "border-bottom-right-radius:3px;" \
                                  "border-bottom-left-radius:3px;" \
                                  "font-size: 16px;" \
                                  "background-color: #933942;" \
                                  "font-family:\"Arial\";} " \
                                  "QPushButton::hover{color:white;}"
                                  );

      _layout->addWidget(_closeButton, 0, 5);
      QLabel* spacer = new QLabel("");
      spacer->setMinimumWidth(20);
      spacer->setStyleSheet("QLabel{border:none;}");
      _layout->addWidget(spacer, 0, 4);
      QObject::connect(_closeButton, &QPushButton::clicked, this, &ActionBar::close_clicked);
    }

    if((actions & Save) == Save) {
      _saveButton = new QPushButton(" save ");
      _saveButton->setFixedHeight(20);
      _saveButton->setEnabled(false);
      _saveButton->setStyleSheet(_DisabledButtonStyle);
      _layout->addWidget(_saveButton, 0, 3);
      QObject::connect(_saveButton,     &QPushButton::clicked, this, &ActionBar::save_clicked);
    }

    if((actions & Validate) == Validate) {
      _validateButton = new QPushButton(" validate ");
      _validateButton->setFixedHeight(20);
      _validateButton->setEnabled(true);
      _validateButton->setStyleSheet(_EnabledButtonStyle);
      _layout->addWidget(_validateButton, 0, 2);
      QObject::connect(_validateButton,     &QPushButton::clicked, this, &ActionBar::validate_clicked);
    }

    if((actions & StartStop) == StartStop) {
      _startStopButton = new QPushButton(START_TEXT);
      _startStopButton->setFixedHeight(20);
      _startStopButton->setEnabled(true);
      _startStopButton->setStyleSheet(_EnabledButtonStyle);
      _layout->addWidget(_startStopButton, 0, 2);
      QObject::connect(_startStopButton,     &QPushButton::clicked, this, &ActionBar::startstop_clicked);
    }

    setStyleSheet("QWidget {padding: 0px;}");
    _layout->setSpacing(0);
    _layout->setContentsMargins(2,2,10,2);
    setLayout(_layout);
  }

void ActionBar::startstop_clicked()
{
  if(_startStopButton->text() == START_TEXT) {
    _startStopButton->setText(STOP_TEXT);
    start_clicked();
  }
  else {
    _startStopButton->setText(START_TEXT);
    stop_clicked();
  }
  
}

void ActionBar::enable_save()
{
  _saveButton->setEnabled(true);
  _saveButton->setStyleSheet(_EnabledButtonStyle);
}

void ActionBar::disable_save()
{
  _saveButton->setEnabled(false);
  _saveButton->setStyleSheet(_DisabledButtonStyle);
}

ActionBar::~ActionBar()
{
  delete _layout;
}


const QString ActionBar::_DisabledButtonStyle{"QPushButton {border: 1px solid #3d415c;border-top-left-radius:2px;border-top-right-radius:2px;border-bottom-right-radius:2px; border-bottom-left-radius:2px;font-size: 16px;color:black;background-color: #3d415c;font-family:\"Arial\";}"};


const QString ActionBar::_EnabledButtonStyle{
                                     "QPushButton {border: 1px solid black;" \
                                     "border-top-left-radius:3px;" \
                                     "border-top-right-radius:3px;" \
                                     "border-bottom-right-radius:3px;" \
                                     "border-bottom-left-radius:3px;" \
                                     "font-size: 16px;" \
                                     "color:black;" \
                                     "background-color: #6274C4;" \
                                     "font-family:\"Arial\";} " \
                                     "QPushButton::hover{color:white;}"
                                       };
