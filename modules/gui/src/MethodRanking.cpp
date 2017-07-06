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
  _methods->clear();
  for(auto& x : ranks) {
    std::string desc = std::to_string(x.invoked_count());
    frenchroast::monitor::pad(desc, 10);
    desc.append(x.descriptor());
    _methods->addItem(new QListWidgetItem(QString::fromStdString(desc)));
  }
}

MethodRanking::MethodRanking()
{
  _methods = new QListWidget();
  _methods->setFont(CodeFont());
  QVBoxLayout* vlayout = new QVBoxLayout();
  vlayout->addWidget(_methods);
  setLayout(vlayout);
  
}

MethodRanking::~MethodRanking()
{
  delete _methods;
}
