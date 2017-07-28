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

#ifndef EDITOR_H
#define EDITOR_H

#include <QTextEdit>
#include <QListWidget>
#include <QSettings>
#include <unordered_map>
#include "FViewer.h"



namespace frenchroast {

  class Editor : public FViewer {

    Q_OBJECT

  private:
    static Editor*  _instance;
    Editor(QWidget*);
    ~Editor();
    QTextEdit*    _edit{nullptr};
    QListWidget*  _message{nullptr};
    std::string   _filename;
    bool          _changesToSave{false};
    std::unordered_map<std::string, std::string> _signals;
    ActionButton* _bsave;
    void add(QString);
    
  public:
    static Editor* instance(QWidget*);
    static void capture();

    std::vector<std::string> lines() const;
    
  signals:
    void validated_hooks(std::vector<std::string>);
    void changed();

    private slots:
      void contents_changed();
      void goto_error_line(QListWidgetItem*);
  public slots:
      void validate_hooks();
      void load_from_file(const std::string&);
      void save();
      void save_as(const std::string&);
      void add_hook(QString);
      void shutdown();
  };

  class MessageItem : public QListWidgetItem {
    int _line;
  public:
    MessageItem(const std::string& str, int line = 0);
    MessageItem()
      {
      }
    int line() const;
  };

}
Q_DECLARE_METATYPE(frenchroast::MessageItem);  
#endif
