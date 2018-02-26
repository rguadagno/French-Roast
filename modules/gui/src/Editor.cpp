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

#include <iostream>
#include <fstream>
#include <QBoxLayout>
#include <QSplitter>
#include <QHeaderView>
#include <QTableWidgetItem>
#include "Editor.h"
#include "CodeFont.h"
#include "HooksSyntax.h"
#include "fr_signals.h"
#include "Util.h"
#include "QUtil.h"
#include "SignalDelegate.h"

namespace frenchroast {
  
  Editor::Editor(QWidget* parent) : FViewer(parent)
  {

    _edit = new QTextEdit();
    _edit->setCursorWidth(8);
    _message = new QListWidget{};
    _message->setStyleSheet("QListWidget {border-top:none;border-bottom: 1px solid #808080;border-left: 1px solid #808080;border-right: 1px solid #808080;background-color: #9e9e9e;} ");
    
    _edit->setFont(CodeFont());
    _edit->setStyleSheet(_settings->value("edit_style").toString());

    _instrumentation = new QTableWidget{};
    _instrumentation->setStyleSheet(_settings->value("traffic_grid_style").toString());
    _instrumentation->verticalHeader()->hide();
    _instrumentation->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    _instrumentation->insertColumn(0);
    _instrumentation->setHorizontalHeaderItem(0, createItem("Signal Instrumentation Status"));
    _instrumentation->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    _instrumentation->setItemDelegateForColumn(0, new SignalDelegate(_instrumentation)); 
    //    _instrumentation->horizontalHeader()->setSectionResizeMode(0,QHeaderView::ResizeToContents);
    
    new HooksSyntax(_edit->document());
    _edit->document()->setModified(false);
    QSplitter* splitter = new QSplitter();
    splitter->setOrientation(Qt::Vertical);
    splitter->setContentsMargins(0,0,0,0);
    splitter->addWidget(_edit);
    splitter->addWidget(_message);
    splitter->addWidget(_instrumentation);
    setup_dockwin("Editor", splitter, false);
    _filename = _settings->value("editor:signal-file", "").toString().toStdString();
    
    if(_filename != "") {
      load_from_file(_filename);
    }
    else {
      _settings->beginGroup("hooks");
      int total = _settings->beginReadArray("unlinked");
      for(int idx = 0; idx < total; idx++) {
        _settings->setArrayIndex(idx);
        add(_settings->value("item").toString());
      }
      _settings->endArray();
      _settings->endGroup();
    }
    _bsave = new ActionButton("Save", false);
    QObject::connect(_actionBar->add(_bsave) ,     &ActionButton::request, this, &frenchroast::Editor::save);
    QObject::connect(_actionBar->add(new ActionButton("Validate")) , &ActionButton::request, this, &frenchroast::Editor::validate_signals);
    QObject::connect(_edit->document(), &QTextDocument::contentsChange,  this,    [&]() {     _changesToSave = true; _bsave->enable(); });
    QObject::connect(_message,          &QListWidget::itemDoubleClicked, this,    &Editor::goto_error_line);
  }

  
  void Editor::goto_error_line(QListWidgetItem* qitem)
  {
     MessageItem* item = dynamic_cast<MessageItem*>(qitem);
     _edit->setFocus();
    _edit->moveCursor(QTextCursor::Start);
    for(int idx = 0; idx < item->line(); idx++) {
      _edit->moveCursor(QTextCursor::Down);
    }
  }

  void Editor::contents_changed()
  {
    _changesToSave = true;
    _validated = false;
    changed();
  }

  
  void Editor::validate_signals()
  {
    if(_validated) {
      std::string outstr = _edit->document()->toPlainText().toStdString();
      validated_signals(frenchroast::split(outstr, "\n"));
      return;
    }
    signal::Signals sigs;
    _message->clear();
    bool validated = true;
    std::string outstr = _edit->document()->toPlainText().toStdString();
    std::vector<std::string> signals_from_editor{frenchroast::split(outstr, "\n")};
    int xline = 0;
    int total = 0;
    for(auto& line : signals_from_editor) {
      try {
        if(sigs.load(line)) {
          ++total;
        }
      }
      catch(std::invalid_argument& e) {
        validated = false;
        MessageItem* item = new MessageItem(e.what(), xline);
        item->setForeground(QColor("#990000"));
        item->setFont(CodeFont());
        _message->addItem(item);

      }
    ++xline;
    }
    if(validated) {
      _validated = true;
      QListWidgetItem* item;
      if(total > 0) {
        item = new MessageItem("validation PASSED.");
      }
      else {
        item = new MessageItem("WARNING: Since no hooks given no signals will be generated.");
      }
      item->setForeground(QColor("#443355"));
      item->setFont(CodeFont());
      _message->addItem(item);
      clearTable(_instrumentation);
      _descriptor_row.clear();
      _host_pid_column.clear();
      for(int idx = _instrumentation->columnCount() -1; idx > 0; idx--) {
        _instrumentation->removeColumn(idx);
      }
      addRow(_instrumentation, createItem(""));
      for(auto& x : get_signals(sigs)) {
        addRow(_instrumentation, createItem(x,Qt::AlignLeft));
        _descriptor_row[x] = _instrumentation->rowCount() - 1;
      }
      validated_signals(signals_from_editor);
    }
  }

  void Editor::update(const monitor::InstrumentationReport& rpt)
  {
    if(_host_pid_column.find(rpt.hostname() + rpt.pid()) == _host_pid_column.end()) {
      _instrumentation->insertColumn(_instrumentation->columnCount());
      int col = _instrumentation->columnCount() -1;
      _instrumentation->setHorizontalHeaderItem(col, createItem("hostname[pid]"));
      _instrumentation->horizontalHeader()->setSectionResizeMode(col,QHeaderView::ResizeToContents);
      _instrumentation->setItem(0, col, new QTableWidgetItem(std::string{rpt.hostname() + "[" + rpt.pid() + "]"}.c_str()));    
      _host_pid_column[rpt.hostname() + rpt.pid()] = col;
    }
    for(auto& x : rpt.valid()) {
      _instrumentation->setItem(_descriptor_row[x], _host_pid_column[rpt.hostname() + rpt.pid()], new QTableWidgetItem("OK"));
    }
    for(auto& x : rpt.invalid()) {
      _instrumentation->setItem(_descriptor_row[x], _host_pid_column[rpt.hostname() + rpt.pid()],new QTableWidgetItem("BAD"));
    }

  }

  
  void Editor::load_from_file(const std::string& filename)
  {
    /*
    _filename = filename;
    std::string text = "";
    std::ifstream in;
    in.open(filename);
    std::string line;
    while (getline(in,line)) {
      text.append(line + "\n");  // <--------- call add_hook instead
    }
    in.close();
    _edit->document()->setPlainText(QString::fromStdString(text));
    */
  }

  void Editor::save()
  {
    if(_filename == "") {
      _settings->beginGroup("hooks");
      _settings->remove("");
      _settings->beginWriteArray("unlinked");
      int idx = 0;
      for(auto& line : lines()) {
        _settings->setArrayIndex(idx++);
        _settings->setValue("item", QString::fromStdString(line));
      }
      _settings->endArray();
      _settings->endGroup();
      _changesToSave = false;
      _bsave->disable();
    }
    else {
      std::ofstream out;
      out.open(_filename);
      QString outstr = _edit->document()->toPlainText();
      out << outstr.toStdString();
      out.close();
      _changesToSave = false;
      _bsave->disable();
    }
  }

  void Editor::save_as(const std::string& filename)
  {
    
  }

  void Editor::add_hook(QString text)
  {
    add(text +    "<ENTER>");
  }
    
  void Editor::add(QString text)
  {
    std::string className = split(text.toStdString(),"::")[0];
    std::string methodName = split(text.toStdString(),"::")[1];
    size_t pos = methodName.find_first_of("<");
    if(pos != std::string::npos) {
      methodName = methodName.substr(0, pos);
    }
    std::string descriptor = className + "::" + methodName;
    if(_signals.count(descriptor) == 1) return;

    _signals[descriptor] = className;
    QString str = _edit->document()->toPlainText();
    if(str.size() > 0 && str[str.size() -1] != '\n') {
      str.append('\n');
    }
    str.append(text);
    _edit->document()->setPlainText(str);
  }
    

  std::vector<std::string> Editor::lines() const
  {
    std::string outstr = _edit->document()->toPlainText().toStdString();
    return {frenchroast::split(outstr, "\n")};
  }

  void Editor::shutdown()
  {
    // no warnings for unsaved changes yet
  }


  Editor* Editor::_instance{nullptr};
  const std::string Editor::FName{"editor"};
  
  Editor::~Editor()
  {
    _instance = nullptr;
  }

  Editor* Editor::instance(QWidget* parent)
  {
    if(_instance != nullptr) return _instance;
    _instance = new Editor(parent);
    restore_win(FName, _settings, _instance, dynamic_cast<QMainWindow*>(parent));
    return _instance;
  }

  void Editor::capture()
  {
     capture_win(FName, _settings, _instance != nullptr ? _instance : nullptr);
  }

  bool Editor::restore_is_required()
  {
    return restore_required(FName, _settings);
  }

  
  MessageItem::MessageItem(const std::string& str, int line) : QListWidgetItem(QString::fromStdString(str)), _line(line)
  {
  }
    
  int MessageItem::line() const
  {
    return _line;
  }
  
  bool Editor::up()
  {
      return _instance != nullptr;
  }


}
