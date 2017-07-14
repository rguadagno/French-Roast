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
    _message->setStyleSheet("QListWidget {border-top:none;border-bottom: 1px solid #808080;border-left: 1px solid #808080;border-right: 1px solid #808080;background-color: #d0e4ed;} QListWidget::item {color: #ba0303;}");
    
    _edit->setFont(CodeFont());
    new HooksSyntax(_edit->document());
    _edit->document()->setModified(false);
    
    QVBoxLayout* vlayout = new QVBoxLayout();
    vlayout->setContentsMargins(0,0,0,0);
    setLayout(vlayout);
    vlayout->addWidget(_edit);
    vlayout->addWidget(_message);

    QObject::connect(_edit->document(), &QTextDocument::contentsChange, this,    &Editor::contents_changed);
    
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
    for(auto& line : hooks) {
      frenchroast::agent::HookValidatorStatus status = frenchroast::agent::validate_hook(line);
      if(!status) {
        validated = false;
        QListWidgetItem* item = new QListWidgetItem(QString::fromStdString(status.msg()));
        item->setFont(CodeFont());
        _message->addItem(item);
      }
    }
    if(validated) {
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

}
