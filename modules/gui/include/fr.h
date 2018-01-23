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

#include <unordered_map>
#include <vector>
#include <string>
#include <QApplication>
#include <QLabel>
#include <QObject>
#include <QMetaType>
#include "Monitor.h"
#include "StackTrace.h"
#include "MethodStats.h"
#include "StackReport.h"
#include "DetailHolder.h"
#include "ClassDetail.h"
#include "JammedReport.h"
#include "SignalReport.h"
#include "TimerReport.h"

Q_DECLARE_METATYPE(std::string);
Q_DECLARE_METATYPE(std::vector<frenchroast::monitor::StackTrace>);
Q_DECLARE_METATYPE(std::vector<frenchroast::monitor::MarkerField>);
Q_DECLARE_METATYPE(std::vector<frenchroast::monitor::MethodStats>);
using SRType =   std::unordered_map<std::string, frenchroast::monitor::StackReport>;

Q_DECLARE_METATYPE(SRType);
Q_DECLARE_METATYPE(MarkerField);
Q_DECLARE_METATYPE(DetailHolder);
Q_DECLARE_METATYPE(std::vector<frenchroast::monitor::ClassDetail>);
Q_DECLARE_METATYPE(frenchroast::monitor::JammedReport);
Q_DECLARE_METATYPE(frenchroast::monitor::SignalReport);
Q_DECLARE_METATYPE(frenchroast::monitor::TimerReport);
Q_DECLARE_METATYPE(frenchroast::monitor::StackReport);
Q_DECLARE_METATYPE(frenchroast::monitor::SignalParams);
Q_DECLARE_METATYPE(frenchroast::monitor::SignalMarkers);
Q_DECLARE_METATYPE(frenchroast::monitor::Descriptor);


class FRListener : public QObject
{
  Q_OBJECT
  
  private:
    std::unordered_map<std::string, int>      _items;
    std::string                               _ip;
    int                                       _port;
    frenchroast::monitor::Monitor<FRListener> _mon;
    int                                       _trafficRate{-1};
    std::vector<std::string>                  _signal_requests;

  public:
    FRListener(const std::string ip, int port, const std::string& opcodFile);
    int getCount(const std::string& item);
    void signal(const frenchroast::monitor::SignalReport&);
    void traffic(std::vector<frenchroast::monitor::StackTrace>);
    void signal_timed(const frenchroast::monitor::TimerReport&);
    void connected(const std::string& host, const std::string& pid);
    void unloaded(const std::string& host, const std::string& pid);
    void ack_profiler_off(const std::string& host, const std::string& pid);
    void ack_profiler_on(const std::string& host, const std::string& pid);
    void ready(const std::string& host, const std::string& pid);
    void request_hooks(const std::string& ipport);
    void jammed(frenchroast::monitor::JammedReport&);

  public slots:
    void init();
    void start_traffic(int);
    void stop_traffic();
    void validated_signals(std::vector<std::string>);
    void start_watch_loading();
    void stop_watch_loading();
    void class_watch(const std::vector<frenchroast::monitor::ClassDetail>& details);
    void turn_on_profiler(const std::string&);
    void turn_off_profiler(const std::string&);
    void reset();
    
  signals:
    void send_signals();
    void method_ranking(std::vector<frenchroast::monitor::MethodStats>);
    void thooked(const frenchroast::monitor::SignalReport&);
    void timersignal(const frenchroast::monitor::TimerReport&);
    void remoteconnected(const std::string& host, const std::string& pid);
    void remoteunloaded(const std::string& host, const std::string& pid);
    void remote_ack_off(const std::string& host, const std::string& pid);
    void remote_ack_on(const std::string& host, const std::string& pid);
    void traffic_signal(const std::vector<frenchroast::monitor::StackTrace>&);
    void class_loaded(const std::vector<frenchroast::monitor::ClassDetail>&);
    void stack_jammed(const frenchroast::monitor::JammedReport&);
    void remote_ready(const std::string&, const std::string&);

};


#endif
