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

#include "Editor.h"
#include "CodeFont.h"
#include "HooksSyntax.h"
#include "HookValidator.h"
#include <QBoxLayout>
#include <QSplitter>
#include <iostream>
#include <fstream>
#include "Util.h"

namespace frenchroast {
  
  Editor::Editor(QWidget* parent) : FViewer(parent)
  {
    _actionBar = new ActionBar(ActionBar::Close | ActionBar::Save | ActionBar::Validate);
    _edit = new QTextEdit();
    _edit->setCursorWidth(8);
    _message = new QListWidget{};
    _message->setStyleSheet("QListWidget {border-top:none;border-bottom: 1px solid #808080;border-left: 1px solid #808080;border-right: 1px solid #808080;background-color: #9e9e9e;} ");
    
    _edit->setFont(CodeFont());
    _edit->setStyleSheet(_settings->value("edit_style").toString());
    new HooksSyntax(_edit->document());
    _edit->document()->setModified(false);
    QVBoxLayout* vlayout = new QVBoxLayout();
    QSplitter* splitter = new QSplitter();
    splitter->setOrientation(Qt::Vertical);
    splitter->setContentsMargins(0,0,0,0);
    splitter->addWidget(_edit);
    splitter->addWidget(_message);
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
    
    QObject::connect(_actionBar, &ActionBar::save_clicked, this, &frenchroast::Editor::save);
    QObject::connect(_actionBar, &ActionBar::validate_clicked,   this, &frenchroast::Editor::validate_hooks);
    QObject::connect(_edit->document(), &QTextDocument::contentsChange,  this,    [&]() {     _changesToSave = true; _actionBar->enable_save(); });
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
    changed();
  }
  
  void Editor::validate_hooks()
  {
    _message->clear();
    bool validated = true;
    std::string outstr = _edit->document()->toPlainText().toStdString();
    std::vector<std::string> hooks{frenchroast::split(outstr, "\n")};
    int xline = 0;
    int total = 0;
    for(auto& line : hooks) {
      frenchroast::agent::HookValidatorStatus status = frenchroast::agent::validate_hook(line);
      if(!status) {
        validated = false;
        MessageItem* item = new MessageItem(status.msg(), xline);
        item->setForeground(QColor("#990000"));
        item->setFont(CodeFont());
        _message->addItem(item);
      }
      else {
        if(!status.is_comment()) {
          ++total;
        }
      }
    ++xline;
    }
    if(validated) {
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
       validated_hooks(hooks);
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
      saved();
    }
    else {
      std::ofstream out;
      out.open(_filename);
      QString outstr = _edit->document()->toPlainText();
      out << outstr.toStdString();
      out.close();
      _changesToSave = false;
      saved();
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
  Editor::~Editor()
  {
        std::cout << "* NULLPTR * " << std::endl;
    _instance = nullptr;
  }

  Editor* Editor::instance(QWidget* parent)
  {
    if(_instance != nullptr) return _instance;
    _instance = new Editor(parent);
    restore_win("editor", _settings, _instance, dynamic_cast<QMainWindow*>(parent));
    return _instance;
  }

  void Editor::capture()
  {
     capture_win("editor", _settings, _instance != nullptr ? _instance : nullptr);
  }


  
  MessageItem::MessageItem(const std::string& str, int line) : QListWidgetItem(QString::fromStdString(str)), _line(line)
  {
  }
    
  int MessageItem::line() const
  {
    return _line;
  }
  


}
