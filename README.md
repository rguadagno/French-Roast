## French-Roast
A pure C++ bytecode instrumentation API for Java class files. 

### Motivation
The JVMTI API provides call-backs to profile for monitor contention (eg `MonitorContendedEnter()` ) but not for objects such as ConcurrentHashMap since its locking mechanism is outside the JVM instruction set. If we would like to understand the performance of the ConcurrentHashMap in an application, the way to do it is to add a JNI call-back hook to the appropriate method such as `scanAndLockForPut` in `ConcurrentHashMap$Segment`.

The purpose of French-Roast is to allow us to insert these light weight JNI hooks into any method. The agent.dll must implement the JNI native method with full JNI name (eg Java_java_util_concurrent_ConcurrentHashMap etc...). We add a static native hook method to java.lang.Package and instrument the method being investigated to call that hook. Hence we only need to code the Java_java_lang_Package_thook() once in our monitor dll.


### Example

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

The location tag can be `<ENTER>`, `<ENTER|EXIT>`, `<EXIT>`, or `<20,132>`
Currently only `<ENTER>` is supported. The `<20,132>` means add the hook at these line numbers (for this to work the Java source must have been compiled with debug information).

What this does is effectivly change the code to:
```Java
   static public void fib(int v1, int v2, int count, final int max) {
      Package.thook();  // <------------------------------------------------ hook ----------
      System.out.print(v1 + " ");
      if ((count + 1 )<= max) {
         fib(v2,v1+v2, count+1,max);
      }
    }
```

Our JVMTI code to handle the callback looks like:
```C++

JNIEXPORT void JNICALL Java_java_lang_Package_thook (JNIEnv * ptr, jclass object)
{
  jvmtiFrameInfo frames[5];
  jint count;
  jvmtiError err;
  jthread aThread;

  genv->GetCurrentThread(&aThread);
  err = genv->GetStackTrace(aThread, 0, 5, frames, &count);
  if (err == JVMTI_ERROR_NONE && count >= 1) {
    char *methodName;
    char *sig;
    char *generic;
    err = genv->GetMethodName(frames[1].method, &methodName,&sig,&generic);
    if (err == JVMTI_ERROR_NONE) {
      std::string methodNameStr{methodName};
      std::string sigStr{sig};
      _rptr.hook(methodNameStr + ":" +sigStr);   // <----- send this over a Socket to server listening
    }
  }
}

```

We can also put hooks into Java classes such as ConcurrentHashMap to facilitate concurrency contention ( which can be monitored for the built in
JVM synchronized, or wait() ).
```
java.util.concurrent.ConcurrentHashMap$Segment::scanAndLockForPut:(java.lang.Object,int,java.lang.Object):java.util.concurrent.ConcurrentHashMap$HashEntry <ENTER>

```





  