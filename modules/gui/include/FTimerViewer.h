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

#ifndef FTIMERVIEWER_H
#define FTIMERVIEWER_H
#include <QWidget>
#include <QSettings>
#include <QListWidget>
#include "FViewer.h"
#include <unordered_map>
#include "FListItem.h"

namespace frenchroast {
  class FTimerViewer : public FViewer {

    Q_OBJECT

    QListWidget*                               _data;
    std::unordered_map<std::string,FListItem*> _descriptors;
    
  public slots:
      void update_time(const std::string& descriptor, long elapsed);

  public:
    FTimerViewer(QSettings&, QWidget*);
  };
}
#endif
