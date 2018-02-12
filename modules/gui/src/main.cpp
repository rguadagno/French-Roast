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

#include <string>
#include <QApplication>
#include <QMessageBox>
#include <QThread>
#include <QSettings>
#include "fr.h"
#include "FRMain.h"
#include "Editor.h"
#include "PersistorFile.h"
#include "Session.h"


int main(int argc, char* argv[])
{
  if (argc != 3 && argc != 4) {
    std::cout << "usage: roaster <ip> <port> [path to hooks file]\nexample: roaster 127.0.0.1 6060 /home/richg/code/hooks.txt" << std::endl;
    exit(0);
  }
  QApplication app(argc,argv);
  QSettings config{frenchroast::monitor::get_env_variable("INI_FULL_PATH","example /home/richg/French-Roast/modules/gui/config/fr.ini"), QSettings::IniFormat};
  std::string path_to_opcodes = frenchroast::monitor::get_env_variable("OPCODES_FULL_PATH","example /home/richg/French-Roast/modules/instrumentation/opcodes.txt");
  char* sdir = std::getenv("SESSION_DIR");
  std::string session_dir = (sdir == nullptr ? "" : sdir);
  
  qRegisterMetaType<std::string>();
  qRegisterMetaType<std::vector<frenchroast::monitor::StackTrace>>();
  qRegisterMetaType<SRType>("std::unordered_map<std::string, frenchroast::monitor::StackReport>");
  qRegisterMetaType<frenchroast::monitor::MarkerField>("frenchroast::monitor::MarkerField");
  qRegisterMetaType<std::vector<frenchroast::monitor::MarkerField>>();
  qRegisterMetaType<std::vector<frenchroast::monitor::MethodStats>>();
  qRegisterMetaType<std::vector<std::string>>();
  qRegisterMetaType<frenchroast::MessageItem>();
  qRegisterMetaType<DetailHolder>();
  qRegisterMetaType<std::vector<frenchroast::monitor::ClassDetail>>();
  qRegisterMetaType<frenchroast::monitor::JammedReport>("frenchroast::monitor::JammedReport");
  qRegisterMetaType<frenchroast::monitor::SignalReport>("frenchroast::monitor::SignalReport");
  qRegisterMetaType<frenchroast::monitor::TimerReport>("frenchroast::monitor::TimerReport");
  qRegisterMetaType<frenchroast::monitor::HeapReport>("frenchroast::monitor::HeapReport");
  
  FRListener roaster{std::string{argv[1]}, atoi(argv[2]), path_to_opcodes};
  QThread* tt = new QThread(&roaster);
  roaster.moveToThread(tt);
  PersistorFile* ptr = new PersistorFile{};
  Session session{ptr};
  FRMain main(config, argc == 4 ? argv[3] : "", session, session_dir);  

  QObject::connect(&roaster, &FRListener::thooked,         &main,    &FRMain::update_list);
  QObject::connect(&roaster, &FRListener::timersignal,     &main,    &FRMain::update_timed_list);
  QObject::connect(&roaster, &FRListener::traffic_signal,  &main,    &FRMain::update_traffic);
  QObject::connect(&roaster, &FRListener::stack_jammed,    &main,    &FRMain::update_jammed);
  QObject::connect(&roaster, &FRListener::heap_changed,    &main,    &FRMain::update_heap);
  QObject::connect(&roaster, &FRListener::class_loaded,    &main,    &FRMain::update_class_viewer);
  QObject::connect(tt,       &QThread::started,            &roaster, &FRListener::init);
  QObject::connect(&app,     &QApplication::aboutToQuit,   &main,    &FRMain::handle_exit);
  QObject::connect(&roaster, &FRListener::remoteconnected, &main,    &FRMain::remote_connected);
  QObject::connect(&roaster, &FRListener::remoteunloaded,  &main,    &FRMain::remote_disconnected);
  QObject::connect(&roaster, &FRListener::remote_ack_off,  &main,    &FRMain::remote_ack_off);
  QObject::connect(&roaster, &FRListener::remote_ack_on,   &main,    &FRMain::remote_ack_on);
  QObject::connect(&roaster, &FRListener::remote_ready,    &main,    &FRMain::handshake);
  QObject::connect(&main,    &FRMain::start_traffic,       &roaster, &FRListener::start_traffic);
  QObject::connect(&main,    &FRMain::stop_traffic,        &roaster, &FRListener::stop_traffic);
  QObject::connect(&main,    &FRMain::start_loading,       &roaster, &FRListener::start_watch_loading);
  QObject::connect(&main,    &FRMain::stop_loading,        &roaster, &FRListener::stop_watch_loading);
  QObject::connect(&main,    &FRMain::reset,               &roaster, &FRListener::reset);
  QObject::connect(&roaster, &FRListener::method_ranking,  &main,    &FRMain::method_ranking);
  QObject::connect(&roaster, &FRListener::send_signals,    &main,    &FRMain::validate_signals);
  QObject::connect(&main,    &FRMain::validated_signals,   &roaster, &FRListener::validated_signals);
  QObject::connect(&main,    &FRMain::turn_on_profiler,    &roaster, &FRListener::turn_on_profiler);
  QObject::connect(&main,    &FRMain::turn_off_profiler,   &roaster, &FRListener::turn_off_profiler);
  
  tt->start();
  main.show();
  main.move(5000,5000);
 return app.exec();
}
