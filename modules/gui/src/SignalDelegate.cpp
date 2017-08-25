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

#include <QPainter>
#include "SignalDelegate.h"

SignalDelegate::SignalDelegate(QObject* parent) : QItemDelegate(parent)
{
  _textDoc = new QTextDocument{};
  _syntax = new SignalSyntax{_textDoc};
}


void SignalDelegate::drawDisplay(QPainter* painter, const QStyleOptionViewItem& option, const QRect& rect, const QString& text) const
{
  Q_UNUSED(option);
    
  _textDoc->setPlainText(text);
  QPixmap pixmap{rect.size()};
  pixmap.fill(Qt::transparent);
  QPainter p{&pixmap};
  _textDoc->drawContents(&p);
  painter->drawPixmap(rect,pixmap);
}







