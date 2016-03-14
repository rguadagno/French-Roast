## French-Roast
A pure C++ bytecode instrumentation API for Java class files. 

### Motivation
The JVMTI API provides call-backs to profile for monitor contention (eg `MonitorContendedEnter()` ) but not for objects such as ConcurrentHashMap since its locking mechanism is outside the JVM instruction set. If we would like to understand the performance of the ConcurrentHashMap in an application, the way to do it is to add a JNI call-back hook to the appropriate method such as `scanAndLockForPut` in `ConcurrentHashMap$Segment`.

The purpose of French-Roast is to allow us to insert these light weight JNI hooks into any method. The agent.dll must implement the JNI native method with full JNI name (eg Java_java_util_concurrent_ConcurrentHashMap etc...). We add a static native hook method to java.lang.Package and instrument the method being investigated to call that hook. Hence we only need to code the Java_java_lang_Package_thook() once in our monitor dll.


### Example 1

Given a Java class that implements a recursive Fibonacci generator:
```Java
package mypackage;

public class Example {
   static public void main(String[] args) {
     fib(1,1,1,10) // print out first 10 numbers in Fibonacci series
   }

   static public void fib(int v1, int v2, int count, final int max) {
      System.out.print(v1 + " ");
      if ((count + 1 )<= max) {
         fib(v2,v1+v2, count+1,max);
      }
    }
}
```
Assume in the above code that the fib method is called from many places in a large system.

First we edit the hooks_config.txt file to look like:
```
mypackage.Example::fib:(int,int,int,int):void                           <ENTER>
```

The location tag can be <ENTER>, <ENTER|EXIT>, <EXIT>, or <20,132>
Currently only <ENTER> is supported. The <20,132> means add the hook at these line numbers (for this to work the Java source must have been compiled with debug information).

What this does is effectivly change the code to:
```
   static public void fib(int v1, int v2, int count, final int max) {
      Package.thook();  // <------------------------------------------------ hook ----------
      System.out.print(v1 + " ");
      if ((count + 1 )<= max) {
         fib(v2,v1+v2, count+1,max);
      }
    }
```





This is done from in the callback JVMTI method ClassFileLoadHook (see modules/monitor/src/Monitor.cpp)

Instrument Package:
```C++
fr.load_from_buffer(class_data);
fr.add_name_to_pool("thook");
fr.add_name_to_pool("()V");
fr.add_native_method("thook", "()V"); 
jint size = fr.size_in_bytes();
jvmtiError err = env->Allocate(size,new_class_data);
*new_class_data_len = size;
fr.load_to_buffer(*new_class_data); 
```
Instrument any method with hook, such as scanAndLockForPut:
```C++
fr.load_from_buffer(class_data);
fr.add_name_to_pool("thook");
fr.add_name_to_pool("()V");
fr.add_method_call("scanAndLockForPut:(Ljava/lang/Object;ILjava/lang/Object;)Ljava/util/concurrent/ConcurrentHashMap$HashEntry;", "java/lang/Package.thook:()V", fr.METHOD_ENTER|fr.METHOD_EXIT);
jint size = fr.size_in_bytes();
jvmtiError err = env->Allocate(size,new_class_data);
*new_class_data_len = size;
fr.load_to_buffer(*new_class_data); 
```






  