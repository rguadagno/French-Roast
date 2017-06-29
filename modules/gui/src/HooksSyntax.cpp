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

#include <QRegularExpression>
#include "HooksSyntax.h"



void HooksSyntax::highlightBlock(const QString& text)
{

  QTextCharFormat pointFormat;
  pointFormat.setFontWeight(QFont::Normal);
  pointFormat.setForeground(Qt::yellow);

  QTextCharFormat commentFormat;
  commentFormat.setFontWeight(QFont::Normal);
  commentFormat.setForeground(Qt::darkGray);

  QTextCharFormat markerFormat;
  markerFormat.setFontWeight(QFont::Normal);
  markerFormat.setForeground(QBrush(QColor(89,89,193)));


  
  QRegularExpression pointReg("<ENTER>|<EXIT>|<TIMER>");
  QRegularExpressionMatchIterator itr = pointReg.globalMatch(text);
  while(itr.hasNext()) {
    QRegularExpressionMatch match = itr.next();
    setFormat(match.capturedStart(), match.capturedLength(), pointFormat);
  }

  
  QRegularExpression markerReg("\\[[A-Za-z\\_0-9 ]*\\]");
  itr = markerReg.globalMatch(text);
  while(itr.hasNext()) {
    QRegularExpressionMatch match = itr.next();
    setFormat(match.capturedStart(), match.capturedLength(), markerFormat);
  }
  QRegularExpression commentReg("^#[A-Za-z\\.:() <>\\[\\]\\_]*");
  itr = commentReg.globalMatch(text);
  while(itr.hasNext()) {
    QRegularExpressionMatch match = itr.next();
    setFormat(match.capturedStart(), match.capturedLength(), commentFormat);
  }


  
}

HooksSyntax::HooksSyntax(QTextDocument* docptr) : QSyntaxHighlighter(docptr)
{
}
