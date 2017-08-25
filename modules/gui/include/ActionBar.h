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

#ifndef ACTIONBAR_H
#define ACTIONBAR_H

#include <bitset>
#include <QWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QGridLayout>
#include "MonitorUtil.h"

class ActionButton : public QPushButton {

Q_OBJECT
  
    const static QString _EnabledButtonStyle;
    const static QString _DisabledButtonStyle;

    QString _onText;
    QString _offText;
    int     _toggleState{0};

    private slots:
      void toggled();
      void actioned();

      public slots:
        void enable();
        void disable();
        
   signals:
      void request(const std::string&);

 public:
      ActionButton(const std::string& text, bool enabled = true);
};


class ActionEdit : public QLineEdit {

  Q_OBJECT

 signals:
  void changed(const std::string&);
  
 public:
  ActionEdit(const char*);
};

class ActionLabel : public QLabel {

    Q_OBJECT

 public:
    ActionLabel(const char*);

};

class ActionBar : public QWidget {
  QGridLayout* _layout{nullptr};
  int          _idx{10};
  QPushButton* _closeButton{nullptr};
  
   Q_OBJECT
 signals:
   void close_clicked();
 public:
    ActionBar(const ActionBar&) = delete;
    ActionBar(const std::bitset<4>& actions = Close);
    ~ActionBar();
    ActionButton* add(ActionButton*);
    ActionEdit*   add(ActionEdit*);
    ActionLabel*  add(ActionLabel*);
    QWidget*      add(QWidget*);
    
  const static std::bitset<4> Close;
};

#endif
