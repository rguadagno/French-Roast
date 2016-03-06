## French-Roast
A pure C++ bytecode instrumentation API (for Java class files). 

### Motivation
The JVMTI API provides call-backs to profile for monitor contention (eg MonitorContendedEnter() ) but not for objects such as ConcurrentHashMap since its locking mechanism is outside the JVM instruction set. Hence we sould like to understand the performance of the ConcurrentHashMap in an application. The way to do that is add a JNI call-back hook to the appropriate method such as scanAndLockForPut in ConcurrnetHashMap$Segment.

The purpose of French-Roast is to allow these light weight JNI hooks into any method. The agent.dll must implement the JNI native method with full JNI name ( eg Java_java_util_concurrent_ConcurrentHashMap etc... ). We add a static native hook method to java.lang.Package and instrument the methid being investigated to call that hook. Hench we only need to code the Java_java_lang_Package_thook() once in our monitor dll.


### Example







  