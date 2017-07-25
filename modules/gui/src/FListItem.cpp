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

#include "FListItem.h"
#include "CodeFont.h"

FListItem::FListItem(const std::string& text, const std::string& total) : _text(text), QListWidgetItem( QString::fromStdString(total) + "   " + QString::fromStdString(text) )
{
  setFont(_font);
}

const std::string& FListItem::gettext() const
{
  return _text;
}



QFont FListItem::_font = CodeFont();
