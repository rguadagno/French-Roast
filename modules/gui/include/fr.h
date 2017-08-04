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
#include <QMetaType>
#include "Monitor.h"
#include "StackTrace.h"
#include "MethodStats.h"
#include "StackReport.h"
#include "DetailHolder.h"
#include "ClassDetail.h"

Q_DECLARE_METATYPE(std::string);
Q_DECLARE_METATYPE(std::vector<frenchroast::monitor::StackTrace>);
Q_DECLARE_METATYPE(std::vector<frenchroast::monitor::MarkerField>);
Q_DECLARE_METATYPE(std::vector<frenchroast::monitor::MethodStats>);
using SRType =   std::unordered_map<std::string, frenchroast::monitor::StackReport>;
Q_DECLARE_METATYPE(SRType);
Q_DECLARE_METATYPE(DetailHolder);
Q_DECLARE_METATYPE(std::vector<frenchroast::monitor::ClassDetail>);


class FRListener : public QObject
{
  Q_OBJECT
  
  private:
    std::unordered_map<std::string, int> _items;
    frenchroast::monitor::Monitor<FRListener> _mon;
    std::string _ip;
    int         _port;
    int         _trafficRate{-1};
    bool        _hooks_request{false};

  public:
    FRListener(const std::string ip, int port, const std::string& opcodFile);
    int getCount(const std::string& item);
    void signal(const std::string& tag, const std::string& tname, int count,  std::vector<std::string>,std::vector<std::string>, std::vector<frenchroast::monitor::MarkerField>, std::unordered_map<std::string, frenchroast::monitor::StackReport>);
    void traffic(std::vector<frenchroast::monitor::StackTrace>&);
    void signal_timed(const std::string& tag, const std::string& tname, long elapsed, int last);
    void connected(const std::string& host, const std::string& pid);
    void unloaded(const std::string& host, const std::string& pid);
    void ready(const std::string& host, const std::string& pid);
    void request_hooks();

  public slots:
    void init();
    void start_traffic(int);
    void stop_traffic();
    void validated_hooks(std::vector<std::string>);
    void start_watch_loading();
    void stop_watch_loading();
    void class_watch(const std::vector<frenchroast::monitor::ClassDetail>& details);
    
  signals:
    void send_hooks();
    void method_ranking(std::vector<frenchroast::monitor::MethodStats>);
    void thooked(const std::string& info,const std::string& tname, int count, const std::vector<std::string>&, const std::vector<std::string>&, const std::vector<frenchroast::monitor::MarkerField>&,  std::unordered_map<std::string, frenchroast::monitor::StackReport>);
    void timersignal(const std::string& info, const std::string& tname, long elapsed);
    void remoteconnected(const std::string& host, const std::string& pid);
    void remoteunloaded(const std::string& host, const std::string& pid);
    void traffic_signal(const std::vector<frenchroast::monitor::StackTrace>&);
    void class_loaded(const std::vector<frenchroast::monitor::ClassDetail>&);
    void remote_ready(const std::string&, const std::string&);

};


#endif
