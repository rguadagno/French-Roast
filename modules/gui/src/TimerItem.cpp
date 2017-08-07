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

#include "TimerItem.h"
#include "MonitorUtil.h"



TimerListener::TimerListener(QTableWidgetItem* item) : _item(item)
{
}

void TimerListener::tick()
{
  using namespace frenchroast::monitor;
  _elapsed += 1000;
  _item->setText(QString::fromStdString(format_millis(_elapsed, FORMAT_HOURS|FORMAT_MINUTES|FORMAT_SECONDS)));
}



TimerItem::TimerItem() : QTableWidgetItem("")
  {
    _timer   = new QTimer{};
    _listener = new TimerListener(this);
    QObject::connect(_timer,&QTimer::timeout, _listener, &TimerListener::tick);
  }


void TimerItem::start()
{
  _timer->start(1000);
}

void TimerItem::stop()
{
  _timer->stop();
}
