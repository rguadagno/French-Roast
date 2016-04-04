CC := cl /c /Zi /EHsc /Ic:\devtools\Catch-master\include /I..\include  /Fobuild\\
build := ..\build

VPATH = src;include;build;..\build;


bin\runner.exe : core Test_Util.obj Test.obj Test_Main.obj Test_OpCode.obj Test_Instruction.obj Test_Method.obj FrenchRoast.obj AccessFlags.obj ConstantPoolComponents.obj StackMapFrame.obj Util.obj Method.obj Exception.obj
	link /out:bin\runner.exe build\Test_Util.obj build\Test.obj build\Test_Main.obj build\Test_OpCode.obj build\Test_Instruction.obj build\Test_Method.obj $(build)\FrenchRoast.obj $(build)\AccessFlags.obj $(build)\ConstantPoolComponents.obj $(build)\StackMapFrame.obj $(build)\Util.obj $(build)\OpCode.obj $(build)\Instruction.obj $(build)\Method.obj $(build)\Exception.obj

core: makefile
	c:\devtools\gnumake.exe -C ..\


Test_Util.obj: Test_Util.cpp
	$(CC) $<

Test.obj: Test.cpp
	$(CC) $<

Test_OpCode.obj: Test_OpCode.cpp
	$(CC) $<

Test_Main.obj: Test_Main.cpp
	$(CC) $<

Test_Instruction.obj: Test_Instruction.cpp
	$(CC) $<

Test_Method.obj: Test_Method.cpp
	$(CC) $<


FrenchRoast.obj :
AccessFlags.obj :
ConstantPoolComponents.obj :
StackMapFrame.obj :
Util.obj :
Method.obj :
Exception.obj :
