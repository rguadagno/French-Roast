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


IFILEOBJECTS = FrenchRoast.obj Util.obj AccessFlags.obj ConstantPoolComponents.obj StackMapFrame.obj OpCode.obj Instruction.obj
FILEOBJECTS = Config.obj WindowsReporter.obj Hooks.obj Monitor.obj  $(IFILEOBJECTS)
OBJECTS = $(addprefix $(BUILDDIR), $(FILEOBJECTS))
IOBJECTS = $(addprefix ..\instrumentation\build\, $(IFILEOBJECTS))

# 
# ---------------------------------------------------------------------------------------------   
#
bin\Monitor.dll: core $(FILEOBJECTS)
	link /out:bin\Monitor.dll /DLL build\Monitor.obj build\Hooks.obj  build\Config.obj build\WindowsReporter.obj  ws2_32.lib $(IOBJECTS)

core: makefile
	c:\devtools\gnumake.exe -C ..\instrumentation

$(BUILDDIR)Config.obj: Config.cpp Config.h
	$(CC) $<

$(BUILDDIR)WindowsReporter.obj: WindowsReporter.cpp Reporter.h
	$(CC) $<

$(BUILDDIR)Hooks.obj: Hooks.cpp Hooks.h
	$(CC) $<

$(BUILDDIR)Monitor.obj: Monitor.cpp Monitor.h Hooks.h
	$(CC) $<



