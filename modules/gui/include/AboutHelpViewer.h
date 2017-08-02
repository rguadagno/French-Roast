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

#ifndef ABOUTVIEWER_H
#define ABOUTVIEWER_H
#include <QWidget>
#include "FViewer.h"
#include "FRStatus.h"


namespace frenchroast {
  class AboutHelpViewer : public FViewer {

    Q_OBJECT

    static AboutHelpViewer*          _instance;
    static const std::string         FName;
    FRStatus*                        _statusMsg;
    AboutHelpViewer(QWidget*);
    ~AboutHelpViewer();
    
  public:
    static AboutHelpViewer* instance(QWidget*);
    static void capture();
    static bool restore_is_required();

    public slots:
      void remote_connected(const std::string& from);
      void remote_disconnected(const std::string& msg);
      void remote_ready();

  };
}
#endif
