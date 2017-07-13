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

#include <QMainWindow>
#include <QLabel>
#include <QListWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QDockWidget>
#include <QSettings>
#include <QTextEdit>
#include <unordered_map>
#include <unordered_set>
#include "fr.h"
#include "StackRow.h"
#include "ActionBar.h"
#include "FRStatus.h"
#include "EnterKeyListener.h"
#include "MethodStats.h"
#include "MethodRanking.h"

class SignalItem;
using VecStr = std::vector<std::string>; 
class DetailHolder {
 public:
 DetailHolder( VecStr ah, VecStr ih, std::vector<frenchroast::monitor::MarkerField> fields) : _argHeaders(ah), _instanceHeaders(ih), _markers(fields)
    {
    }
  DetailHolder()
    {
    }
  
  std::vector<std::string> _argHeaders;
  std::vector<std::string> _instanceHeaders;
  std::vector<frenchroast::monitor::MarkerField> _markers;
};

class FRMain : public QMainWindow {
  Q_OBJECT

 public:
  FRMain(QSettings&, const std::string&);
  const static std::string  EditHooksWindow;
  const static std::string  SignalWindow;
  const static std::string  TimerWindow;
  const static std::string  RankingWindow;
  const static std::string  TrafficWindow;

 private:
  std::unordered_map<std::string, QDockWidget*>                 _docks;
  static std::unordered_map<std::string,  void (FRMain::*)()  > _dockbuilders;
  QSettings&    _settings;
  bool          _exit{false};
  bool          _ok_to_send_hooks{false};
  QTextEdit*    _hooksEditor{nullptr};
  QListWidget*  _messageList{nullptr};
  QListWidget*  _list;
  QListWidget*  _timedlist;
  QTableWidget* _traffic;
  QPushButton*  _buttonStartTraffic;
  QLineEdit*    _rate;
  FRStatus*      _statusMsg;
  EnterKeyListener* _trafficEnterKeyListener;

  std::unordered_map<std::string, std::unordered_map<std::string,SignalItem*>> _descriptorsPerDock;
  std::unordered_map<std::string,  std::unordered_map<std::string, int>>      _detailItems;
  std::unordered_map<std::string,QTableWidget*>             _detailLists;
  std::unordered_map<std::string, std::vector<std::string>> _detailItemsPerList;
  std::unordered_map<std::string, DetailHolder>             _detailDescriptors;
  std::unordered_map<std::string,StackRow*>                 _traffic_rows;
  std::unordered_map<std::string, int>                      _traffic_keys;
  MethodRanking*                                            _rankings;
  std::string                                               _hooksfile;
  
  std::string format_markers(const std::string markers);
  void update_detail_list(std::string,QTableWidget*, const DetailHolder& detailholder);
  QWidget* build_traffic_viewer(QTableWidget* grid, QPushButton* bstart, QLineEdit* rate);

  QDockWidget* setup_dock_window(const std::string& title, QWidget* wptr, ActionBar* aptr, const std::string& wstyle,QDockWidget::DockWidgetFeatures features = QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);

  void capture_dock(const std::string& dockname);
  void restore_dock_win(const std::string& docname);
  void bring_up_dock_if_required(const std::string dockname);
  void connect_dock_win(QMenu* mptr, const std::string& actionname, const std::string& docname);
  void view_dockwin(const std::string& title, const std::string& dockname, QWidget* wptr);
  
 signals:
  
  void hooks_saved();
  void start_traffic(int);
  void stop_traffic();
  void validated_hooks(std::vector<std::string>);
  
 public slots:
   void validate_and_send_hooks();
   bool validate_hooks();
   void method_ranking(std::vector<frenchroast::monitor::MethodStats> ranks);
   void remote_connected(const std::string& msg);
   void remote_disconnected(const std::string& msg);
   void view_hooks_editor();
   void view_signals();
   void view_timers();
   void view_ranking();
   void view_traffic();
   void save_hooks();
   void add_hook(QString);
   void show_detail(QListWidgetItem* item);
   void destroy_list(QObject* obj);
   void reset_editor(QObject* obj);
   void handle_exit();
   void update_list(std::string, std::string, std::string, int, const std::vector<std::string> ,  const std::vector<std::string>, const std::vector<frenchroast::monitor::MarkerField>);
   void update_timed_list(std::string  descriptor, std::string, long elapsed);
   void update_traffic(const std::vector<frenchroast::monitor::StackTrace>& stacks);
   void update_traffic_rate();
   void handshake();
};



#endif
