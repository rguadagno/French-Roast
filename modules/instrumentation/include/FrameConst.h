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
//    along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
//
#ifndef FRAMECONST_H
#define FRAMECONST_H
namespace frenchroast { namespace stackmapframe {
    const BYTE same                 = 1;
    const BYTE same_extended        = 2;
    const BYTE same_local_one_stack = 3;
    const BYTE append               = 4;
    const BYTE chop                 = 5;
    const BYTE full                 = 6;

  }
}
#endif
