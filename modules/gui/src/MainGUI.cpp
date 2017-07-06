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

#include <QTWidgets/QApplication>
#include <QTWidgets/QLabel>
#include <QTWidgets/QHBoxLayout>
#include <QTWidgets/QListWidget>
#include <QTWidgets/QMessageBox>
#include <QTWidgets/QStatusBar>
#include <QTWidgets/QSizePolicy>
#include <QTWidgets/QPushButton>
#include <QTWidgets/QTableWidget>
#include <QTWidgets/QDockWidget>
#include <QTCore/QObject>
#include <QtGlobal>
#include <QThread>
#include <QHeaderView>
#include <QAction>
#include <QFont>
#include <string>
#include "fr.h"
#include "FRMain.h"
#include "MonitorUtil.h"
#include "StackRow.h"
#include "HooksSyntax.h"
#include <QSettings>
#include <QDesktopWidget>
#include <QMenuBar>
#include <QToolBar>
#include <algorithm>
#include "CodeFont.h"

int main(int argc, char* argv[]) {

  QSettings config{frenchroast::monitor::get_env_variable("INI_FULL_PATH","example /home/richg/French-Roast/modules/gui/config/fr.ini"), QSettings::IniFormat};
  std::string path_to_opcodes = frenchroast::monitor::get_env_variable("OPCODES_FULL_PATH","example /home/richg/French-Roast/modules/instrumentation/opcodes.txt");
  qRegisterMetaType<std::string>();
  qRegisterMetaType<std::vector<frenchroast::monitor::StackTrace>>();
  qRegisterMetaType<std::vector<frenchroast::monitor::MarkerField>>();
  
  QApplication app(argc,argv);

  if (argc != 4) {
    QMessageBox box;
    box.setText("usage: roaster <ip> <port> <path to hooks file>\nexample: roaster 127.0.0.1 6060 /home/richg/code/hooks.txt");
    box.exec();
    exit(0);
  }

  FRListener roaster{std::string{argv[1]}, atoi(argv[2]), path_to_opcodes, argv[3]};
  QThread* tt = new QThread(&roaster);

  roaster.moveToThread(tt);
  FRMain main(&roaster, config, argv[3]);
  
  QObject::connect(&roaster, &FRListener::thooked,        &main,    &FRMain::update_list);
  QObject::connect(&roaster, &FRListener::timersignal,    &main,    &FRMain::update_timed_list);
  QObject::connect(&roaster, &FRListener::traffic_signal, &main,    &FRMain::update_traffic);
  QObject::connect(tt,       &QThread::started,           &roaster, &FRListener::init);
  QObject::connect(&app,     &QApplication::aboutToQuit,  &main,    &FRMain::handle_exit);

  tt->start();
  main.setWindowTitle("French Roast");
  main.show();

 return app.exec();
}

class SignalItem : public QListWidgetItem {
  std::string  _text;
  static QFont _font;
public:
  SignalItem(const std::string& text, const std::string& total) : _text(text), QListWidgetItem( QString::fromStdString(text) + "  " + QString::fromStdString(total))
  {
    setFont(_font);
  }

  const std::string& gettext() const
  {
    return _text;
  }

};


const std::string  FRMain::SignalWindow    = "signals";
const std::string  FRMain::EditHooksWindow = "edit-hooks";
const std::string  FRMain::TimerWindow     = "timers";
const std::string  FRMain::RankingWindow   = "rankings";
const std::string  FRMain::TrafficWindow   = "traffic";

std::unordered_map< std::string,  void (FRMain::*)()  > FRMain::_dockbuilders {{FRMain::SignalWindow,    &FRMain::view_signals},
                                                                               {FRMain::EditHooksWindow, &FRMain::view_hooks_editor},
                                                                               {FRMain::TimerWindow,     &FRMain::view_timers},
                                                                               {FRMain::RankingWindow,   &FRMain::view_ranking},
                                                                               {FRMain::TrafficWindow,   &FRMain::view_traffic}
      };

QFont SignalItem::_font = CodeFont();
QFont StackRow::_font = CodeFont();

FRMain::FRMain(FRListener* listener, QSettings& settings, const std::string& path_to_hooks) : _settings(settings), _listener(listener), _hooksfile(path_to_hooks)
{
  QDesktopWidget* dw = QApplication::desktop();
  int height = dw->availableGeometry(dw->primaryScreen()).height() * 0.2;
  int width  = dw->availableGeometry(dw->primaryScreen()).width() * 0.7;

  QMenu* mptr = menuBar()->addMenu("&View");
  connect_dock_win(mptr, "Signals",         SignalWindow);
  connect_dock_win(mptr, "Timers",          TimerWindow);
  connect_dock_win(mptr, "Hooks Editor",    EditHooksWindow);
  connect_dock_win(mptr, "Method Rankings", RankingWindow);
  connect_dock_win(mptr, "Traffic",         TrafficWindow);
  
  resize(width,height);

  resize(_settings.value("main:width", width).toInt(),
         _settings.value("main:height", height).toInt());

  move(_settings.value("main:xpos", 0).toInt(),
       _settings.value("main:ypos", 0).toInt());


  _list               = new QListWidget;
  _timedlist          = new QListWidget;
  _traffic            = new QTableWidget;
  _buttonStartTraffic = new QPushButton{"Start"};
  _rate               = new QLineEdit;
  _trafficEnterKeyListener = new EnterKeyListener;
  _rankings = new MethodRanking;
  _traffic->installEventFilter(_trafficEnterKeyListener);

  
  for(auto& x : _dockbuilders) {
    bring_up_dock_if_required(x.first);    
  }
  _statusMsg = new FRStatus{statusBar()};
  
  QObject::connect(_list,                    &QListWidget::itemDoubleClicked,  this,       &FRMain::show_detail);
  QObject::connect(_buttonStartTraffic,      &QPushButton::clicked,            this,       &FRMain::update_traffic_rate);
  QObject::connect(_trafficEnterKeyListener, &EnterKeyListener::enterkey,      this,       &FRMain::add_hook);
  QObject::connect(listener,                 &FRListener::remoteconnected,     _statusMsg, &FRStatus::remote_connected);
  QObject::connect(listener,                 &FRListener::remoteunloaded,      _statusMsg, &FRStatus::remote_disconnected);
  QObject::connect(listener,                 &FRListener::remote_ready,        this,       &FRMain::handshake);
  QObject::connect(this    ,                 &FRMain::update_method_ranking,   _rankings,  &MethodRanking::update);

  statusBar()->addPermanentWidget(_statusMsg,10);
  _statusMsg->waiting_for_connection();
}


void FRMain::handshake()
{
  if(_buttonStartTraffic->text() == "Stop") {
    _listener->start_traffic(atoi(_rate->text().toStdString().c_str()));
  }
}

QDockWidget* FRMain::setup_dock_window(const std::string& title, QWidget* wptr, ActionBar* actionptr, const std::string& wstyle, QDockWidget::DockWidgetFeatures features)
{

  wptr->setStyleSheet(_settings.value(QString::fromStdString(wstyle)).toString());
  QDockWidget* holder = new QDockWidget(QString::fromStdString(title), this);
  holder->setStyleSheet(_settings.value("dock_widget_style").toString());
  QLabel* sigLabel = new QLabel(QString::fromStdString(title));
  QWidget* titlebar = new QWidget();
  QGridLayout* layout = new QGridLayout();
  
  layout->addWidget(sigLabel,1,1);
  layout->setColumnStretch(1,10);
  layout->setContentsMargins(1,1,1,1);
  layout->addWidget(actionptr,1, 5);
  titlebar->setLayout(layout);
  titlebar->setStyleSheet(_settings.value("dock_win_header_style").toString());
  sigLabel->setStyleSheet(_settings.value("dock_title_style").toString());
  holder->setTitleBarWidget(titlebar);
  holder->setAttribute(Qt::WA_DeleteOnClose);
  holder->setFeatures(features);
  QVBoxLayout* vlayout = new QVBoxLayout();
  holder->setLayout(vlayout);
  holder->setWidget(wptr);
  return holder;
}

void FRMain::connect_dock_win(QMenu* mptr, const std::string& actionname, const std::string& docname)
{
  QAction* act = mptr->addAction(QString::fromStdString(actionname));
  QObject::connect(act, &QAction::triggered, this, _dockbuilders[docname]);
}

void FRMain::restore_dock_win(const std::string& dockname)
{
  _docks[dockname]->setFloating(true);
  _settings.setValue(QString::fromStdString(dockname + ":up"), true);
  _docks[dockname]->resize(_settings.value(QString::fromStdString(dockname + ":width"), width()).toInt(),
                           _settings.value(QString::fromStdString(dockname + ":height"), 200).toInt()
                           );

  _docks[dockname]->move(_settings.value(QString::fromStdString(dockname + ":xpos"), width() /2).toInt(),
                         _settings.value(QString::fromStdString(dockname + ":ypos"), height() /2).toInt());
  addDockWidget(Qt::TopDockWidgetArea,    _docks[dockname]);
}


QDockWidget* FRMain::build_traffic_viewer(QTableWidget* grid, QPushButton* bstart, QLineEdit* rate)
{
  QWidget* buttonHolder = new QWidget;
  QHBoxLayout* hlayout = new QHBoxLayout();
  QLabel* desc = new QLabel{QString::fromStdString("rate (millisec):")};
  desc->setAlignment(Qt::AlignRight);
  desc->setStyleSheet(_settings.value("descriptive_text_style").toString());
  
  buttonHolder->setLayout(hlayout);
  rate->setText("100");
  bstart->setSizePolicy({QSizePolicy::Fixed,QSizePolicy::Fixed});
  rate->setSizePolicy({QSizePolicy::Fixed,QSizePolicy::Fixed});

  hlayout->addWidget(desc,0,Qt::AlignLeft);
  hlayout->addWidget(rate,0,Qt::AlignLeft);
  hlayout->addWidget(bstart,0,Qt::AlignLeft);
  
  buttonHolder->setStyleSheet(_settings.value("button_holder_style").toString());
  rate->setStyleSheet(_settings.value("data_entry_style").toString());

  rate->setFixedWidth(50);
  rate->setInputMask("0000");
  rate->setAlignment(Qt::AlignRight);

  bstart->setStyleSheet(_settings.value("traffic_start_style").toString());
  bstart->setFixedWidth(70);


  QDockWidget* docwin = new QDockWidget(QString::fromStdString("Traffic"), this);
  QWidget* holder = new QWidget();

  docwin->setWidget(holder);
  
  QVBoxLayout* vlayout = new QVBoxLayout();
  vlayout->setSpacing(0);
  holder->setLayout(vlayout);
   
  grid->setStyleSheet(_settings.value("traffic_grid_style").toString());
  grid->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
  grid->horizontalHeader()->hide();
  grid->verticalHeader()->hide();
  grid->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  vlayout->addWidget(grid);

  QLabel* sigLabel = new QLabel(QString::fromStdString("Traffic"));
  QWidget* titlebar = new QWidget();
  QBoxLayout* layout = new QBoxLayout(QBoxLayout::LeftToRight);
  layout->setSpacing(0);
  layout->addWidget(buttonHolder,1,Qt::AlignLeft);
  layout->addWidget(sigLabel,1, Qt::AlignLeft);
  
  titlebar->setLayout(layout);
  titlebar->setStyleSheet(_settings.value("dock_win_header_style").toString());
  sigLabel->setStyleSheet(_settings.value("dock_title_style").toString());
  docwin->setTitleBarWidget(titlebar);

  holder->setStyleSheet(_settings.value("zero_border_style").toString());
  docwin->setStyleSheet(_settings.value("button_holder_style").toString());
  return docwin;
}


QWidget* setup_central(QWidget* lists)
{
  QWidget* holder = new QWidget;
  holder->setStyleSheet("QWidget {background-color: black;}");
  QVBoxLayout* vlayout = new QVBoxLayout();
  vlayout->setSpacing(0);
  holder->setLayout(vlayout);
  vlayout->addWidget(lists);
  return holder;
}


void FunctionPoint::set_decorated_name(const QString& name)
{
  _name = name;
}

QString FunctionPoint::get_name() const
{
  return _name;
}



void FRMain::bring_up_dock_if_required(const std::string dockname)
{
  if(_settings.value(QString::fromStdString(dockname + ":up")).toBool() == true) {
    (this->*_dockbuilders[dockname])();
  }
}

void FRMain::view_traffic()
{
  if(_docks.count(TrafficWindow) == 1) return;
  _docks[TrafficWindow] = build_traffic_viewer(_traffic, _buttonStartTraffic, _rate);
  _traffic->insertColumn(0);
  restore_dock_win(TrafficWindow);
}

void FRMain::view_ranking()
{

  if(_docks.count(RankingWindow) == 1) return;
  _docks[RankingWindow] = setup_dock_window("Ranking", _rankings,     new ActionBar(), "list_style", QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
  restore_dock_win(RankingWindow);
}

void FRMain::view_signals()
{
  if(_docks.count(SignalWindow) == 1) return;
  
  _docks[SignalWindow] = setup_dock_window("Signals", _list,     new ActionBar(), "list_style", QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
  restore_dock_win(SignalWindow);

}

void FRMain::view_timers()
{
  if(_docks.count(TimerWindow) == 1) return;
  _docks[TimerWindow] = setup_dock_window("Timers", _timedlist, new ActionBar(), "list_style", QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);

   restore_dock_win(TimerWindow);
}



void FRMain::view_hooks_editor()
{
   if(_docks.count(EditHooksWindow) == 1) return;
    _hooksEditor = new QTextEdit();

    std::string text = "";
    std::ifstream in;
    in.open(_hooksfile);
    std::string line;
    while (getline(in,line)) {
      text.append(line + "\n");
    }
    in.close();
  
    _hooksEditor->document()->setPlainText(QString::fromStdString(text));
    
    _hooksEditor->setFont(CodeFont());
    HooksSyntax* syntax = new HooksSyntax(_hooksEditor->document());
    _hooksEditor->document()->setModified(false);
    
    QObject::connect(_hooksEditor, &QTextEdit::destroyed, this, &FRMain::reset_editor);
    ActionBar* abar = new ActionBar(ActionBar::Close | ActionBar::Save);

    QDockWidget* editdoc = setup_dock_window("hooks", _hooksEditor, abar, "edit_style");
    _docks[EditHooksWindow] = editdoc;

    restore_dock_win(EditHooksWindow);
    
    QObject::connect(abar,                     &ActionBar::close_clicked,      editdoc, &QDockWidget::close);
    QObject::connect(abar,                     &ActionBar::close_clicked,      this,    &FRMain::edit_hooks_closed);
    QObject::connect(abar,                     &ActionBar::save_clicked,       this,    &FRMain::save_hooks);
    QObject::connect(this,                     &FRMain::hooks_saved ,          abar,    &ActionBar::disable_save);
    QObject::connect(_hooksEditor->document(), &QTextDocument::contentsChange, abar,    &ActionBar::enable_save);
 }

void FRMain::save_hooks()
{
  if(_hooksEditor == nullptr) return;

  std::ofstream out;
  out.open(_hooksfile);
  QString outstr = _hooksEditor->document()->toPlainText();
  out << outstr.toStdString();
  out.close();
  hooks_saved();
}


void FRMain::add_hook()
{
  if(_hooksEditor == nullptr) return;
  QString str = _hooksEditor->document()->toPlainText() + "\n" + _traffic->currentItem()->text() +    "<ENTER>";
  _hooksEditor->document()->setPlainText(str);
}

void FRMain::show_detail(QListWidgetItem* item)
{
  SignalItem* ptr = dynamic_cast<SignalItem*>(item);
  std::string sname = ptr->gettext();
  if(_detailLists.count(sname) > 0 || sname.find("[M]") == std::string::npos) {
    return;
  }
  _detailLists[sname] = new QListWidget();
  _detailLists[sname]->setObjectName(QString::fromStdString(sname));
  QObject::connect(_detailLists[sname], &QListWidget::destroyed, this, &FRMain::destroy_list);
  ActionBar* abar = new ActionBar(ActionBar::Close);
  
  QDockWidget* dockwin = setup_dock_window(sname, _detailLists[sname], abar ,"list_style");
  QObject::connect(abar, &ActionBar::close_clicked, dockwin, &QDockWidget::close);
  addDockWidget(Qt::TopDockWidgetArea, dockwin);
  update_detail_list(_detailLists[ sname ], _detailDescriptors[sname]);
}

void FRMain::reset_editor(QObject* obj)
{
  _hooksEditor = nullptr;
}

void FRMain::destroy_list(QObject* obj)
{
  if(_exit) return;
  std::string name = obj->objectName().toStdString();
  if(_detailItemsPerList.count(name) == 1) {
    for(auto& x : _detailItemsPerList[name]) {
      _detailItems.erase(x);
    }
    _detailItemsPerList.erase(name);
  }
  _detailLists.erase(name);
}


void FRMain::update_traffic_rate()
{
  if(_buttonStartTraffic->text() == "Start") {
    _listener->start_traffic(atoi(_rate->text().toStdString().c_str()));
    _buttonStartTraffic->setText("Stop");
  }
  else {
    //    _listener->stop_traffic();
    _buttonStartTraffic->setText("Start");
    update_method_ranking(_listener->stop_traffic());
  }
}



void FRMain::update_list(std::string ltype, std::string  descriptor, std::string tname, int count, const std::vector<frenchroast::monitor::MarkerField>& markers)
{
  if(markers.size() > 0) {
    descriptor = "[M] " + descriptor;
  }
  else {
    descriptor = "    " + descriptor;
  }
  tname = "[ " + tname + " ]";
  frenchroast::monitor::pad(descriptor, 50);
  frenchroast::monitor::pad(tname, 10);
  
  descriptor.append(tname);
  _detailDescriptors[descriptor] = markers;
  if (_descriptors.count(descriptor) == 0 ) {
    _descriptors[descriptor] = new SignalItem(descriptor, frenchroast::monitor::ntoa(count,5,' '));
    _list->addItem(_descriptors[descriptor]);
  }
  else {
    _descriptors[descriptor]->setText(QString::fromStdString(descriptor) + "  " + QString::fromStdString(frenchroast::monitor::ntoa(count,5, ' ')));
      if(_detailLists.count(descriptor) == 1) { 
        update_detail_list(_detailLists[descriptor], markers);
      }
  }
}

void FRMain::update_detail_list(QListWidget* list, const std::vector<frenchroast::monitor::MarkerField>& markers)
{
  for(auto& item : markers) {
    if(_detailItems.count(item._descriptor) == 0) {
      _detailItems[item._descriptor] = new SignalItem(frenchroast::monitor::pad(format_markers(item._descriptor),50), frenchroast::monitor::ntoa(item._count,5,' '));
      list->addItem(_detailItems[item._descriptor]);
      _detailItemsPerList[ list->objectName().toStdString() ].push_back(item._descriptor);
    }
    else {
      //@@ called over even if values same....
      _detailItems[item._descriptor]->setText(QString::fromStdString(frenchroast::monitor::pad(format_markers(item._descriptor),50)) + QString::fromStdString(frenchroast::monitor::ntoa(item._count,5,' ')));      
    }
  }
}

std::string FRMain::format_markers(const std::string markers)
{
  using namespace frenchroast::monitor;
  using namespace frenchroast;
  std::string rv = "";
  for(auto& itempair : frenchroast::split(markers,";")) {
    if(itempair.find(":") == std::string::npos) continue;
    rv.append(  pad( split(itempair,":")[0] + "[ " + split(itempair,":")[1] + " ] ", 20));
  }
  return rv;
}

void FRMain::update_traffic(const std::vector<frenchroast::monitor::StackTrace>& stacks)
{
  for(auto& x : stacks) {
    int currRow = _traffic->rowCount();
    if(_traffic_rows.count(x.thread_name()) == 0) {
      _traffic->insertRow(currRow);
      StackRow* sr = new StackRow{x.thread_name(), currRow, _traffic, _traffic_keys};
      _traffic_rows[x.thread_name()] = sr;
    }
    _traffic_rows[x.thread_name()]->add(x);
  }
}



void FRMain::update_timed_list(std::string  descriptor, std::string tname, long elapsed)
{

  std::string desc{descriptor};
  tname = "[ " + tname + " ]";
  frenchroast::monitor::pad(desc, 50);
  frenchroast::monitor::pad(tname, 10);
  desc.append(tname);
  if (_descriptors.count(descriptor) == 0 ) {
    _descriptors[descriptor] = new SignalItem(desc, frenchroast::monitor::format_millis(elapsed));
    _timedlist->addItem(_descriptors[descriptor]);
  }
  else
    _descriptors[descriptor]->setText(QString::fromStdString(desc) + "  " + QString::fromStdString(frenchroast::monitor::format_millis(elapsed)));
}

void FRMain::edit_hooks_closed()
{
  _docks[EditHooksWindow] = nullptr;
}

void FRMain::handle_exit()
{
  _exit = true;
  for(auto& x : _dockbuilders) {
    capture_dock(x.first);
  }
}

void FRMain::capture_dock( const std::string& dockname)
{
  _settings.setValue("main:width",  width());
  _settings.setValue("main:height",  height());
  _settings.setValue("main:xpos",   pos().x());
  _settings.setValue("main:ypos",   pos().y());
  
  if(_docks.count(dockname) == 1 && _docks[dockname] != nullptr) {
    _settings.setValue(QString::fromStdString(dockname + ":up"),    true);
    _settings.setValue(QString::fromStdString(dockname + ":width"),  _docks[dockname]->width());
    _settings.setValue(QString::fromStdString(dockname + ":height"), _docks[dockname]->height());
    _settings.setValue(QString::fromStdString(dockname + ":xpos"),   _docks[dockname]->pos().x());
    _settings.setValue(QString::fromStdString(dockname + ":ypos"),   _docks[dockname]->pos().y());
  }
  else {
    _settings.setValue(QString::fromStdString(dockname + ":up"),    false);
  }
}
