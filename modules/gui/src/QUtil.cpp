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

#include "QUtil.h"
#include "CodeFont.h"

QTableWidgetItem* createItem(QTableWidgetItem* ditem)
{
  ditem->setFont(CodeFont());
  ditem->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
  return ditem;
}


QTableWidgetItem* createItem(int value)
{
  QTableWidgetItem* ditem = new QTableWidgetItem(QString::fromStdString(std::to_string(value)));
  ditem->setFont(CodeFont());
  ditem->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
  return ditem;
}

QTableWidgetItem* createItem(const std::string& value, Qt::Alignment align)
{
  QTableWidgetItem* ditem = new QTableWidgetItem(QString::fromStdString(value));
  ditem->setFont(CodeFont());
  ditem->setTextAlignment(align);
  ditem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
  return ditem;
}

int addRow(QTableWidget* table, QTableWidgetItem* item1, QTableWidgetItem* item2, QTableWidgetItem* item3, QTableWidgetItem* item4)
{
  int row = table->rowCount();
  table->insertRow(row);
  table->setItem(row,0,item1);
  if(item2 != nullptr) {
    table->setItem(row,1, item2);
  }
  if(item3 != nullptr) {
    table->setItem(row,2, item3);
  }
  
  if(item4 != nullptr) {
    table->setItem(row,3, item4);
  }
    
  return row;
}

void clearTable(QTableWidget* table, bool clearcols)
{
  table->clearContents();
  while(table->rowCount() > 0) {
    table->removeRow(0);
  }
  if(clearcols) {
  while(table->columnCount() > 0) {
    table->removeColumn(0);
  }
    
  }
}
