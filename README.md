## French-Roast
A pure C++ bytecode instrumentation API for Java class files. 

### Motivation
The JVMTI API provides call-backs to profile for monitor contention (eg `MonitorContendedEnter()` ) but not for objects such as ConcurrentHashMap since its locking mechanism is outside the JVM instruction set. If we would like to understand the performance of the ConcurrentHashMap in an application, the way to do it is to add a JNI call-back hook to the appropriate method such as `scanAndLockForPut` in `ConcurrentHashMap$Segment`.

The purpose of French-Roast is to allow us to insert these light weight JNI hooks into any method. The agent.dll must implement the JNI native method with full JNI name (eg Java_java_util_concurrent_ConcurrentHashMap etc...). We add a static native hook method to java.lang.Package and instrument the method being investigated to call that hook. Hence we only need to code the Java_java_lang_Package_thook() once in our monitor dll.


### Example







  