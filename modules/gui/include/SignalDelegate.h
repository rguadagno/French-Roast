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

#ifndef SIGDEL_H
#define SIGDEL_H


#include <QItemDelegate>
#include <QTextDocument>
#include "SignalSyntax.h"

class SignalDelegate : public QItemDelegate {

Q_OBJECT

 public:
 SignalDelegate(QObject*);

 protected:
 void drawDisplay(QPainter*, const QStyleOptionViewItem&, const QRect&, const QString&) const;

 private:
 QTextDocument* _textDoc;
 SignalSyntax*   _syntax;





};




#endif
