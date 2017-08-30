// copyright (c) 2016-2017 Richard Guadagno
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

#include <string>
#include "fr.h"
#include "FRMain.h"
#include "MonitorUtil.h"
#include "FSignalViewer.h"
#include "FTimerViewer.h"
#include "FClassViewer.h"
#include "Editor.h"
#include "TrafficViewer.h"
#include "JammedViewer.h"
#include "AboutHelpViewer.h"

using namespace frenchroast;


QSettings* FViewer::_settings = nullptr;

FRMain::FRMain( QSettings& settings, const std::string& path_to_hooks) : _settings(settings), _hooksfile(path_to_hooks)
{
  FViewer::setSettings(&settings);
  restore();
}


void FRMain::validate_hooks(const std::string& ipport)
{
  if(Editor::up()) {
    Editor::instance(this)->bring_to_top();
  }
  else {
    QObject::connect(Editor::instance(this), &frenchroast::Editor::validated_hooks,  this,    &FRMain::validated_hooks);
  }
  Editor::instance(this)->validate_hooks(ipport);
}


void FRMain::handshake(const std::string& host, const std::string& ip)
{
  AboutHelpViewer::instance(this)->remote_ready(host, ip);
  if(_watchTraffic) {
    start_traffic(TrafficViewer::instance(this)->traffic_rate());
  }
  if(_watchLoading) {
    start_loading();
  }

}

void FRMain::remote_connected(const std::string& host, const std::string& pid)
{
  AboutHelpViewer::instance(this)->remote_connected(host,pid);
}

void FRMain::remote_disconnected(const std::string& host, const std::string& pid)
{
  AboutHelpViewer::instance(this)->remote_disconnected(host,pid);
}


void FRMain::view_traffic()
{
  if(TrafficViewer::up()) {
    TrafficViewer::instance(this)->bring_to_top();
  }
  else {
    QObject::connect(TrafficViewer::instance(this), &frenchroast::TrafficViewer::start_watching, this,  &FRMain::start_watching_traffic);
    QObject::connect(TrafficViewer::instance(this), &frenchroast::TrafficViewer::stop_watching,  this,  &FRMain::stop_watching_traffic);
    QObject::connect(TrafficViewer::instance(this), &frenchroast::TrafficViewer::add_signal,     this,  &FRMain::add_hook);
    connect_common_listeners(TrafficViewer::instance(this));
  }
}

void FRMain::view_classviewer()
{
  if(FClassViewer::up()) {
    FClassViewer::instance(this)->bring_to_top();
  }
  else {
    QObject::connect(FClassViewer::instance(this), &frenchroast::FClassViewer::start_watching, this,  &FRMain::start_watch_loading);
    QObject::connect(FClassViewer::instance(this), &frenchroast::FClassViewer::stop_watching, this,   &FRMain::stop_watch_loading);
    QObject::connect(FClassViewer::instance(this), &frenchroast::FClassViewer::add_signal, this,      &FRMain::add_hook);
    QObject::connect(FClassViewer::instance(this), &frenchroast::FClassViewer::closed, this,          &FRMain::stop_watch_loading);
    connect_common_listeners(FClassViewer::instance(this));
  }
}

void FRMain::view_jammed()
{
  if(JammedViewer::up()) {
    JammedViewer::instance(this)->bring_to_top();
  }
  else {
    QObject::connect(JammedViewer::instance(this), &JammedViewer::add_signal, this,  &FRMain::add_hook);
    connect_common_listeners(JammedViewer::instance(this));
  }
}

void FRMain::start_watch_loading()
{
  _watchLoading = true;
  start_loading();
}

void FRMain::stop_watch_loading()
{
   stop_loading();
   _watchLoading = false;
}


void FRMain::connect_common_listeners(FViewer* instance)
{
  QObject::connect(instance, &frenchroast::FViewer::signal_viewer,    this,  &FRMain::view_signals);
  QObject::connect(instance, &frenchroast::FViewer::timer_viewer,     this,  &FRMain::view_timers);
  QObject::connect(instance, &frenchroast::FViewer::editor_viewer,    this,  &FRMain::view_hooks_editor);
  QObject::connect(instance, &frenchroast::FViewer::traffic_viewer,   this,  &FRMain::view_traffic);
  QObject::connect(instance, &frenchroast::FViewer::jammed_viewer,    this,  &FRMain::view_jammed);
  QObject::connect(instance, &frenchroast::FViewer::classload_viewer, this,  &FRMain::view_classviewer);
  QObject::connect(instance, &frenchroast::FViewer::about_viewer,     this,  &FRMain::view_about);
  QObject::connect(instance, &frenchroast::FViewer::reset,            this,  &FRMain::reset_viewers);
  QObject::connect(instance, &frenchroast::FViewer::exit_fr,          this,  &FRMain::exit_fr);
}

void FRMain::view_about()
{
  if(AboutHelpViewer::up()) {
    AboutHelpViewer::instance(this)->bring_to_top();
  }
  else {
    QObject::connect(AboutHelpViewer::instance(this),  &frenchroast::AboutHelpViewer::turn_on_profiler, this, &FRMain::turn_on_profiler);
    QObject::connect(AboutHelpViewer::instance(this),  &frenchroast::AboutHelpViewer::turn_off_profiler, this, &FRMain::turn_off_profiler);
    connect_common_listeners(AboutHelpViewer::instance(this));
  }
}

void FRMain::view_signals()
{
  if(FSignalViewer::up()) {
    FSignalViewer::instance(this)->bring_to_top();
  }
  else {
    QObject::connect(FSignalViewer::instance(this),  &frenchroast::FSignalViewer::view_detail_request, this, &FRMain::show_detail);
    connect_common_listeners(FSignalViewer::instance(this));
  }
}


void FRMain::view_timers()
{
  if(FTimerViewer::up()) {
    FTimerViewer::instance(this)->bring_to_top();
  }
  else {
    connect_common_listeners(FTimerViewer::instance(this));
  }
}

void FRMain::view_hooks_editor()
{
  if(Editor::up()) {
    Editor::instance(this)->bring_to_top();
  }
  else {
    QObject::connect(Editor::instance(this), &frenchroast::Editor::validated_hooks,  this,    &FRMain::validated_hooks);
    connect_common_listeners(Editor::instance(this));
  }
}

void FRMain::add_hook(QString txt)
{
  Editor::instance(this)->add_hook(txt);
}

void FRMain::show_detail(const std::string& descriptor)
{
  std::cout << "SHOW DETAL" << std::endl;
  DetailViewer* dv = DetailViewer::instance(this, descriptor);
  QObject::connect(this, &FRMain::update_detail_list, dv, &DetailViewer::update);
  QDockWidget* dock = *frenchroast::FSignalViewer::instance(this);
  dv->move(dock->x() + 50, dock->y() + 50 ); 
  update_detail_list(descriptor, _detailDescriptors[descriptor]);
}

void FRMain::update_class_viewer(const std::vector<frenchroast::monitor::ClassDetail>& details)
{
  FClassViewer::instance(this)->update(details);
}

void FRMain::update_jammed(const frenchroast::monitor::JammedReport& rpt)
{
  JammedViewer::instance(this)->update(rpt);
}

void FRMain::start_watching_traffic(int rate)
{
  _watchTraffic = true;
  _trafficRate = rate;
  start_traffic(rate);
}

void FRMain::stop_watching_traffic()
{
  _watchTraffic = false;
  stop_traffic();
}


void FRMain::method_ranking(std::vector<frenchroast::monitor::MethodStats> ranks)
{
  TrafficViewer::instance(this)->update_ranking(ranks);
}

void FRMain::update_list(std::string  descriptor, std::string tname, int count,
                         const std::vector<std::string> argHeaders,  const std::vector<std::string> instanceHeaders, 
                         const std::vector<frenchroast::monitor::MarkerField> markers, std::unordered_map<std::string, frenchroast::monitor::StackReport> stacks)
{
  if(_exit) return;

  tname = "[ " + tname + " ] ";
  frenchroast::monitor::pad(descriptor, 50);
  frenchroast::monitor::pad(tname, 10);

  descriptor = tname + descriptor;
  _detailDescriptors[descriptor] = DetailHolder{count, argHeaders, instanceHeaders, markers, stacks};
  frenchroast::FSignalViewer::instance(this)->update_count(descriptor, count);
  update_detail_list(descriptor, _detailDescriptors[descriptor]);
  
}

void FRMain::update_traffic(const std::vector<frenchroast::monitor::StackTrace>& stacks)
{
  if(!_exit) {
    TrafficViewer::instance(this)->update_traffic(stacks);
  }
}

void FRMain::update_timed_list(std::string  descriptor, std::string tname, long elapsed)
{
  std::string desc{descriptor};
  tname = "[ " + tname + " ]";
  frenchroast::monitor::pad(desc, 50);
  frenchroast::monitor::pad(tname, 10);
  desc = tname + desc;
  FTimerViewer::instance(this)->update_time(desc, elapsed);
}

void FRMain::exit_fr()
{
  QCoreApplication::quit();
}

void FRMain::handle_exit()
{
  _exit = true;

  FSignalViewer::capture();
  FTimerViewer::capture();
  Editor::capture();
  FClassViewer::capture();
  TrafficViewer::capture();
  JammedViewer::capture();
  AboutHelpViewer::capture();
}


void FRMain::restore_if_required(const bool required, void (FRMain::* func)(), bool& result)
{
  if(required) {
    (this->*func)();
    result = true;
  }
}

void FRMain::restore()
{
  bool winup = false;
  restore_if_required(FSignalViewer::restore_is_required(),   &FRMain::view_signals, winup);
  restore_if_required(FTimerViewer::restore_is_required(),    &FRMain::view_timers, winup);
  restore_if_required(FClassViewer::restore_is_required(),    &FRMain::view_classviewer, winup);
  restore_if_required(Editor::restore_is_required(),          &FRMain::view_hooks_editor, winup);
  restore_if_required(TrafficViewer::restore_is_required(),   &FRMain::view_traffic, winup);
  restore_if_required(JammedViewer::restore_is_required(),    &FRMain::view_jammed, winup);
  restore_if_required(AboutHelpViewer::restore_is_required(), &FRMain::view_about, winup);

  if(!winup) {
    view_about();
  }
}

void FRMain::reset_viewers()
{
  FSignalViewer::reset();
  FTimerViewer::reset();
  FClassViewer::reset();
  TrafficViewer::reset();
  JammedViewer::reset();
  reset();
}

