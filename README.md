## French-Roast #### A pure C++ bytecode instrumentation API ( for Java class files ). 

The purpose of French-Roast is to allow light weight JNI hooks into any method ( including Java itself e.g. HashMap ).


### Example



### Build For Windows ( using GNU make and MSVC ( 64 bit target )
#### open cmd window


copy jvmti.h and jni.h , jni_md.h to your include dir



To run tests.
download Catch
edit makefile, add Catch locataion to CC
copy Catch
set env JAVA_CLASSES
  example:
  