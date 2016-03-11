FILEOBJECTS = FrenchRoast.obj Util.obj AccessFlags.obj ConstantPoolComponents.obj StackMapFrame.obj OpCode.obj
OBJECTS = $(addprefix ..\..\instrumentation\build\, $(FILEOBJECTS))

CC := cl /c /Zi /EHsc /Ic:\devtools\Catch-master\include /I..\include  /Fobuild\\

build := ..\build

VPATH = src;include;build;..\build;..\instrumentation\build;


bin\runner.exe : Test.obj Hooks.obj
	link /out:bin\runner.exe build\Test.obj $(build)\Hooks.obj $(OBJECTS)

Test.obj: Test.cpp
	$(CC) src\Test.cpp

Hooks.obj :

