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
#include <iostream>
#include <fstream>
#include "Util.h"

namespace frenchroast {
  Editor::Editor() : QWidget()
  {
    _edit = new QTextEdit();
    _edit->setCursorWidth(8);
    
    _message = new QListWidget{};
    _message->setStyleSheet("QListWidget {border-top:none;border-bottom: 1px solid #808080;border-left: 1px solid #808080;border-right: 1px solid #808080;background-color: #9e9e9e;} ");
    
    _edit->setFont(CodeFont());
    new HooksSyntax(_edit->document());
    _edit->document()->setModified(false);
    
    QVBoxLayout* vlayout = new QVBoxLayout();
    vlayout->setContentsMargins(0,0,0,0);
    setLayout(vlayout);
    vlayout->addWidget(_edit);
    vlayout->addWidget(_message);

    QObject::connect(_edit->document(), &QTextDocument::contentsChange,  this,    &Editor::contents_changed);
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
    changed();
  }
  
  void Editor::validate_hooks()
  {
    _message->clear();
    bool validated = true;
    std::string outstr = _edit->document()->toPlainText().toStdString();
    std::vector<std::string> hooks{frenchroast::split(outstr, "\n")};
    int xline = 0;
    for(auto& line : hooks) {
      frenchroast::agent::HookValidatorStatus status = frenchroast::agent::validate_hook(line);
      if(!status) {
        validated = false;
        MessageItem* item = new MessageItem(QString::fromStdString(status.msg()), xline);
        item->setForeground(QColor("#990000"));
        item->setFont(CodeFont());
        _message->addItem(item);
      }
    ++xline;
    }
    if(validated) {
       QListWidgetItem* item = new QListWidgetItem("validation PASSED.");
       item->setForeground(QColor("#443355"));
       item->setFont(CodeFont());
       _message->addItem(item);
       validated_hooks(hooks);
    }
  }

  void Editor::load_from_file(const std::string& filename)
  {
    _filename = filename;
    std::string text = "";
    std::ifstream in;
    in.open(filename);
    std::string line;
    while (getline(in,line)) {
      text.append(line + "\n");
    }
    in.close();
    _edit->document()->setPlainText(QString::fromStdString(text));
  }

  void Editor::save()
  {
    if(_filename != "") {
      std::ofstream out;
      out.open(_filename);
      QString outstr = _edit->document()->toPlainText();
      out << outstr.toStdString();
      out.close();
      saved();
    }
  }

  void Editor::save_as(const std::string& filename)
  {
    
  }

  void Editor::add(QString text)
  {
    QString str = _edit->document()->toPlainText() + "\n" + text +    "<ENTER>";
    _edit->document()->setPlainText(str);
  }


  MessageItem::MessageItem(QString str, int line) : QListWidgetItem(str), _line(line)
  {
  }
    
  int MessageItem::line() const
  {
    return _line;
  }


}
