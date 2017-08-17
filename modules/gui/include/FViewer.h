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

#ifndef FVIEWER_H
#define FVIEWER_H
#include <QWidget>
#include <QListWidgetItem>
#include <QSettings>
#include <QDockWidget>
#include <QMainWindow>
#include "ActionBar.h"

namespace frenchroast {
  
  
  class FViewer : public QWidget {

    Q_OBJECT
    
    QDockWidget*      _dock;
    QWidget*          _parent;
    QListWidgetItem*  _title;
    static int        _opencount;
    
  public:
    FViewer(QWidget*);
    virtual ~FViewer();
    operator QDockWidget*();
    static  void setSettings(QSettings*);
    void resize_win(int width, int height);
    void move(int x, int y);
    void update_title(const std::string&);

  signals:
     void closed();
     void signal_viewer();
     void timer_viewer();
     void editor_viewer();
     void traffic_viewer();
     void classload_viewer();
     void jammed_viewer();
     void about_viewer();
     void exit_fr();
     void reset();
     
     
  protected:

    std::string         _name;
    static QSettings*   _settings;
    ActionBar*   _actionBar;
    void setup_dockwin(const std::string& title, QWidget* wptr, bool codeMode);
  };

  void capture_win(const std::string& name, QSettings* settings, FViewer* win);
  void restore_win(const std::string& name, QSettings* settings, FViewer* win, QMainWindow*);
  bool restore_required( const std::string& name, QSettings*);
}
#endif
