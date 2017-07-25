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

#include <QWidget>
#include <QPushButton>
#include <QGridLayout>
#include <bitset>

class ActionBar : public QWidget {
  QGridLayout* _layout{nullptr};
  QPushButton* _closeButton{nullptr};
  QPushButton* _saveButton{nullptr};
  QPushButton* _validateButton{nullptr};
  QPushButton* _startStopButton{nullptr};
  
  const static QString _DisabledButtonStyle;
  const static QString _EnabledButtonStyle;


  
   Q_OBJECT
  
  signals:
  void save_clicked();
  void close_clicked();
  void validate_clicked();
  void start_clicked();
  void stop_clicked();
    

  public slots:
    void enable_save();
    void disable_save();
    void startstop_clicked();
public:
  ActionBar(const ActionBar&) = delete;
  ActionBar(const std::bitset<4>& actions = None);
  ~ActionBar();

  const static std::bitset<4> Save;
  const static std::bitset<4> Close;
  const static std::bitset<4> None;
  const static std::bitset<4> Validate;
  const static std::bitset<4> StartStop;

  const static QString START_TEXT;
  const static QString STOP_TEXT;
  
};

#endif
