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
CCI := cl /c /Zi /EHsc /Iinclude /I..\instrumentation\include  /Fo..\\instrumentation\\build\\
RMCMD := del

VPATH = src;include;build;..\instrumentation\build;..\instrumentation\src;build;..\instrumentation\include;

BUILDDIR := build/


IFILEOBJECTS = FrenchRoast.obj Util.obj AccessFlags.obj ConstantPoolComponents.obj StackMapFrame.obj OpCode.obj
FILEOBJECTS = Monitor.obj $(IFILEOBJECTS)
OBJECTS = $(addprefix $(BUILDDIR), $(FILEOBJECTS))
IOBJECTS = $(addprefix ..\instrumentation\build\, $(IFILEOBJECTS))

# 
# ---------------------------------------------------------------------------------------------   
#
bin\Monitor.dll: $(FILEOBJECTS)
	link /out:bin\Monitor.dll /DLL build\Monitor.obj $(IOBJECTS)


FrenchRoast.obj : FrenchRoast.cpp FrenchRoast.h
	$(CCI) $<

ConstantPoolComponents.obj : ConstantPoolComponents.cpp ConstantPool.h
	$(CCI) $<

StackMapFrame.obj : StackMapFrame.cpp StackMapFrame.h
	$(CCI) $<

AccessFlags.obj : AccessFlags.cpp AccessFlags.h
	$(CCI) $<

Util.obj : Util.cpp Util.h
	$(CCI) $<

OpCode.obj : OpCode.cpp OpCode.h
	$(CCI) $<


$(BUILDDIR)Monitor.obj: Monitor.cpp Monitor.h
	$(CC) src\Monitor.cpp
