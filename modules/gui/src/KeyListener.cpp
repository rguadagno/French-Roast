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

#include <QKeyEvent>
#include "KeyListener.h"

bool KeyListener::eventFilter(QObject* obj, QEvent* event)
{
  if(event->type() == QEvent::KeyPress) {
    QKeyEvent* key = static_cast<QKeyEvent*>(event);
    if( (key->key() == Qt::Key_Enter) || (key->key() == Qt::Key_Return) ) {
      enterkey();
      return true;
    }
    if( key->key() == Qt::Key_S) {
      signalkey();
      return true;
    }

  }

  return QObject::eventFilter(obj,event);
}

