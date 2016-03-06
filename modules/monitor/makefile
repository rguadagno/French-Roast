# copyright (c) 2016 Richard Guadagno
# contact: rrguadagno@gmail.com
#
# This file is part of French-Roast
#
#    French-Roast is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    French-Roast is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
#

CC := cl /c /Zi /EHsc /Iinclude /I..\instrumentation\include  /Fobuild\\
RMCMD := del

VPATH = src;include;build;..\instrumentation\build;

BUILDDIR := build/

FILEOBJECTS = Monitor.obj FrenchRoast.obj Util.obj AccessFlags.obj ConstantPoolComponents.obj StackMapFrame.obj
OBJECTS = $(addprefix $(BUILDDIR), $(FILEOBJECTS))
LOCALOBJECTS = $(addprefix build\, $(FILEOBJECTS))

# 
# ---------------------------------------------------------------------------------------------   
#
bin\Monitor.dll: $(FILEOBJECTS)
	link /out:bin\Monitor.dll /DLL $^

$(BUILDDIR)Monitor.obj: Monitor.cpp Monitor.h
	$(CC) src\Monitor.cpp
