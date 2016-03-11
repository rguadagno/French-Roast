CC := cl /c /Zi /EHsc /Ic:\devtools\Catch-master\include /I..\include  /Fobuild\\
build := ..\build

VPATH = src;include;build;..\build;


bin\runner.exe : Test.obj FrenchRoast.obj AccessFlags.obj ConstantPoolComponents.obj StackMapFrame.obj Util.obj
	link /out:bin\runner.exe build\Test.obj $(build)\FrenchRoast.obj $(build)\AccessFlags.obj $(build)\ConstantPoolComponents.obj $(build)\StackMapFrame.obj $(build)\Util.obj $(build)\OpCode.obj

Test.obj: Test.cpp
	$(CC) src\Test.cpp

FrenchRoast.obj :
AccessFlags.obj :
ConstantPoolComponents.obj :
StackMapFrame.obj :
Util.obj :

