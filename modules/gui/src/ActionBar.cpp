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
#include <QSizePolicy>
#include <QLabel>
#include "Util.h"

const std::bitset<4> ActionBar::Close{"0001"};

ActionButton::ActionButton(const std::string& text, bool enabled) : QPushButton(QString::fromStdString(frenchroast::split(text,"|")[0]))
{
  setFixedHeight(20);
  setEnabled(true);
  setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
  if(enabled) {
    setEnabled(true);
    setStyleSheet(_EnabledButtonStyle);
  }
  else {
    setEnabled(false);
    setStyleSheet(_DisabledButtonStyle);
  }

  _onText = QString::fromStdString(frenchroast::split(text,"|")[0]);
  if(text.find("|") != std::string::npos) {
    _offText = QString::fromStdString(frenchroast::split(text,"|")[1]);
    QObject::connect(this,     &QPushButton::clicked, this, &ActionButton::toggled);    
  }
  else {
    QObject::connect(this,     &QPushButton::clicked, this, &ActionButton::actioned);    
  }
}

void ActionButton::actioned()
{
  request(text().toStdString());
}

void ActionButton::toggled()
{
  if(_toggleState == 0) {
    _toggleState = 1;
    request(text().toStdString());
    setText(_offText);
  }
  else {
    _toggleState = 0;
    request(text().toStdString());
    setText(_onText);
  }
}


void ActionButton::enable()
{
  setEnabled(true);
  setStyleSheet(_EnabledButtonStyle);
}

void ActionButton::disable()
{
  setEnabled(false);
  setStyleSheet(_DisabledButtonStyle);
}

ActionEdit::ActionEdit(const char* text) : QLineEdit(text)
{
  setStyleSheet("QLineEdit {qproperty-alignment: AlignRight;padding: 0px; border: 1px solid black;background-color: #C9C9C9;font-size: 16px;font-family: \"Arial\"}  ");
  setFixedWidth(50);
  setFixedHeight(20);
  setInputMask("0000");
  setAlignment(Qt::AlignRight);
  setTextMargins(0,0,0,0);
}


ActionButton* ActionBar::add(ActionButton* button)
{
  _layout->addWidget(button, 0, _idx--, Qt::AlignRight);

  return button;
}

ActionEdit* ActionBar::add(ActionEdit* button)
{
  _layout->addWidget(button, 0, _idx--, Qt::AlignRight);
  return button;
}

ActionBar::ActionBar(const std::bitset<4>& actions)
  {
    _layout = new QGridLayout();
    
    if((actions & Close) == Close) {
      _idx = 3;
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
      //      _layout->addWidget(spacer, 0, 4);
      QObject::connect(_closeButton, &QPushButton::clicked, this, &ActionBar::close_clicked);
    }

    setStyleSheet("QWidget {padding: 0px;}");
    _layout->setSpacing(2);
    _layout->setContentsMargins(2,2,10,2);
    setLayout(_layout);
  }


ActionBar::~ActionBar()
{
  delete _layout;
}


const QString ActionButton::_DisabledButtonStyle{"QPushButton {border: 1px solid #3d415c;border-top-left-radius:2px;border-top-right-radius:2px;border-bottom-right-radius:2px; border-bottom-left-radius:2px;font-size: 16px;color:black;background-color: #3d415c;font-family:\"Arial\";}"};



const QString ActionButton::_EnabledButtonStyle{
                                     "QPushButton {border: 1px solid black;" \
                                     "border-top-left-radius:3px;" \
                                     "border-top-right-radius:3px;" \
                                     "border-bottom-right-radius:3px;" \
                                     "border-bottom-left-radius:3px;" \
                                     "font-size: 16px;" \
                                     "color:black;" \
                                     "padding-left:5px;" \
                                     "padding-right:5px;"        \
                                     "background-color: #6274C4;" \
                                     "font-family:\"Arial\";} " \
                                     "QPushButton::hover{color:white;}"
                                       };

