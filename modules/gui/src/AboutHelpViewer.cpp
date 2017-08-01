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

#include "AboutHelpViewer.h"
#include <QTableWidget>

namespace frenchroast {

AboutHelpViewer::AboutHelpViewer(QWidget* parent) : FViewer(parent)
  {
    setup_dockwin("French-Roast", new QTableWidget(), false);
  }



  AboutHelpViewer* AboutHelpViewer::_instance{nullptr};

  AboutHelpViewer::~AboutHelpViewer()
  {
    _instance = nullptr;
  }

  AboutHelpViewer* AboutHelpViewer::instance(QWidget* parent)
  {
    if(_instance != nullptr) return _instance;
    _instance = new AboutHelpViewer(parent);
    restore_win("about", _settings, _instance, dynamic_cast<QMainWindow*>(parent));
    return _instance;
  }



  void AboutHelpViewer::capture()
  {
     capture_win("about", _settings, _instance != nullptr ? _instance : nullptr);
  }

}
