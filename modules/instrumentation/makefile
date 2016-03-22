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

CC := cl /c /Zi /EHsc /Iinclude  /Fobuild\\
RMCMD := del

VPATH = src;include;build

BUILDDIR := build/

FILEOBJECTS = FrenchRoast.obj Util.obj AccessFlags.obj ConstantPoolComponents.obj StackMapFrame.obj OpCode.obj Instruction.obj
OBJECTS = $(addprefix $(BUILDDIR), $(FILEOBJECTS))
LOCALOBJECTS = $(addprefix build\, $(FILEOBJECTS))

# 
# ---------------------------------------------------------------------------------------------   
#

.PHONY : compall
compall : $(FILEOBJECTS)

$(BUILDDIR)FrenchRoast.obj: FrenchRoast.cpp FrenchRoast.h Info.h Attribute.h StackMapFrame.h OpCode.h Instruction.h
	$(CC) $<

$(BUILDDIR)AccessFlags.obj: AccessFlags.cpp  AccessFlags.h
	$(CC) $<

$(BUILDDIR)ConstantPoolComponents.obj: ConstantPoolComponents.cpp  ConstantPool.h
	$(CC) $<

$(BUILDDIR)StackMapFrame.obj: StackMapFrame.cpp  StackMapFrame.h
	$(CC) $<

$(BUILDDIR)OpCode.obj: OpCode.cpp  OpCode.h
	$(CC) $<

$(BUILDDIR)Util.obj: Util.cpp  Util.h
	$(CC) $<

$(BUILDDIR)Instruction.obj: Instruction.cpp  Instruction.h
	$(CC) $<

.PHONY : clean
clean :	
	$(RMCMD) $(LOCALOBJECTS)
