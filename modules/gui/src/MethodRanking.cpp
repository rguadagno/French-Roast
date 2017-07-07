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


#include "MethodRanking.h"
#include "MonitorUtil.h"
#include "CodeFont.h"

void MethodRanking::update(std::vector<frenchroast::monitor::MethodStats> ranks)
{
  clear();
  for(auto& x : ranks) {
    std::string desc = std::to_string(x.invoked_count());
    frenchroast::monitor::pad(desc, 10);
    desc.append(x.descriptor());
    addItem(new QListWidgetItem(QString::fromStdString(desc)));
  }
}

MethodRanking::MethodRanking()
{
  setFont(CodeFont());
}


QString MethodRanking::getEnterMethod()
{
  std::string line = currentItem()->text().toStdString();
  return QString::fromStdString(line.substr(line.find_last_of(" ") + 1));
}
