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

#ifndef FRLIST_H
#define FRLIST_H

#include <QTWidgets/QApplication>
#include <QTWidgets/QLabel>
#include <QTCore/QObject>
#include <unordered_map>
#include <string>



Q_DECLARE_METATYPE(std::string);

class FRListener : public QObject
{
  Q_OBJECT
  
  private:
    std::unordered_map<std::string, int> _items;

  public:
    int getCount(const std::string& item);
    void signal(const std::string& tag, int count);
    void signal_timed(const std::string& tag, long elapsed, int last);
    void connected(const std::string& addr);

    public slots:
    void init();

  signals:
    void thooked(const std::string& ltype, const std::string& info,int count);
    void timersignal(const std::string& ltype, const std::string& info,long elapsed, int last);
    void remoteconnected(const std::string& addr);



};
#endif
