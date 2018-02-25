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

#ifndef MAINWIN_H
#define MAINWIN_H

#include <unordered_map>
#include <QMainWindow>
#include <QLabel>
#include <QListWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QDockWidget>
#include <QSettings>
#include <QTextEdit>
#include "fr.h"
#include "ActionBar.h"
#include "KeyListener.h"
#include "MethodStats.h"
#include "StackReport.h"
#include "DetailHolder.h"
#include "FViewer.h"
#include "Session.h"
#include "TimerReport.h"
#include "HeapReport.h"

using namespace frenchroast::session;
class FRMain : public QMainWindow {
    Q_OBJECT

 public:
    FRMain(QSettings&, const std::string&, Session&, const std::string& session_dir);

 private:
  QSettings&                                                    _settings;
  std::string                                                   _hooksfile;  
  Session&                                                      _session;
  std::string                                                   _session_default_dir;
  bool                                                          _exit{false};
  bool                                                          _ok_to_send_hooks{false};
  std::unordered_map<std::string, DetailHolder>                 _detailDescriptors;
  std::unordered_map<std::string, SignalReport>                 _signalReports;
  std::unordered_map<std::string, HeapReport>                   _heapReports;
  bool                                                          _watchTraffic{false};
  bool                                                          _watchLoading{false};
  int                                                           _trafficRate{100};
  void restore();
  void connect_common_listeners(frenchroast::FViewer* instance);
  void restore_if_required(const bool required, void (FRMain::* func)(), bool& result);
  
 signals:

  void start_loading();
  void stop_loading();
  void hooks_saved();
  void start_traffic(int);
  void stop_traffic();
  void validated_signals(std::vector<std::string>);
  void update_detail_list(std::string,   DetailHolder* detailholder);
  void update_heap_detail(std::string,   HeapReport*);
  void turn_on_profiler(const std::string& hostname_pid);
  void turn_off_profiler(const std::string& hostname_pid);
  void reset();

  
 public slots:
   void update_class_viewer(const std::vector<frenchroast::monitor::ClassDetail>&);
   void update_jammed(const frenchroast::monitor::JammedReport&);
   void update_heap(const frenchroast::monitor::HeapReport&);
   void start_watch_loading();
   void stop_watch_loading();
   void validate_signals();
   void method_ranking(std::vector<frenchroast::monitor::MethodStats> ranks);
   void remote_connected(const std::string& host, const std::string& pid);
   void remote_disconnected(const std::string& host, const std::string& pid);
   void remote_ack_off(const std::string& host, const std::string& pid);
   void remote_ack_on(const std::string& host, const std::string& pid);
   void view_hooks_editor();
   void view_signals();
   void view_timers();
   void view_traffic();
   void view_classviewer();
   void view_jammed();
   void view_heap();
   void view_about();
   void exit_fr();
   void add_hook(QString);
   void show_detail(const std::string& descriptor);
   void show_heap_detail(const std::string& class_name);
   void handle_exit();
   void update_list(const frenchroast::monitor::Signal&);
   void update_timed_list(const frenchroast::monitor::TimerReport& rpt);
   void update_traffic(const std::vector<frenchroast::monitor::StackTrace>& stacks);
   void start_watching_traffic(int);
   void stop_watching_traffic();
   void handshake(const std::string& host, const std::string& ip);
   void reset_viewers();
   void session_save();
   void session_save_as();
   void session_load();
   
};


#endif
