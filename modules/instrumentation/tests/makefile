CC := cl /c /Zi /EHsc /Ic:\devtools\Catch-master\include /I..\include  /Fobuild\\
build := ..\build

VPATH = src;include;build;..\build;


bin\runner.exe : core Test.obj Test_Main.obj Test_OpCode.obj Test_Instruction.obj FrenchRoast.obj AccessFlags.obj ConstantPoolComponents.obj StackMapFrame.obj Util.obj
	link /out:bin\runner.exe build\Test.obj build\Test_Main.obj build\Test_OpCode.obj build\Test_Instruction.obj $(build)\FrenchRoast.obj $(build)\AccessFlags.obj $(build)\ConstantPoolComponents.obj $(build)\StackMapFrame.obj $(build)\Util.obj $(build)\OpCode.obj $(build)\Instruction.obj

core: makefile
	c:\devtools\gnumake.exe -C ..\

Test.obj: Test.cpp
	$(CC) $<

Test_OpCode.obj: Test_OpCode.cpp
	$(CC) $<

Test_Main.obj: Test_Main.cpp
	$(CC) $<

Test_Instruction.obj: Test_Instruction.cpp
	$(CC) $<


FrenchRoast.obj :
AccessFlags.obj :
ConstantPoolComponents.obj :
StackMapFrame.obj :
Util.obj :

