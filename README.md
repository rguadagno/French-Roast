## French-Roast
A Java application profiler + bytecode instrumentation API written in C++ using Qt.

### Motivation
You are a line manager responsible for the applications your team develops. You have limited time and cannot be
intimate with every technical detail. Use French-Roast to profile your applications and discover what is really going on...


### Features
- Hot Stack discovery including monitor indicators
- Hot Method discovery
- Jammed (lock contention) Stack viewer
- Signals (byte code instrumentation JNI callbacks) for any method on entry and exit
  - method arguments and stack traces per signal
  - class instance variable values per signal
- Timers providing elapsed execution for any method
- Heap lifecycle monitoring for user selected classes
- Signal Editor with wildcard method selection
- Loaded classes viewer (displays as classes are loaded)
- Remote profiling (only agent dll or so lib required on target machine(s)
- Multiple JVM clients can be profiled at same time
  - ability to turn on/off profiling for specific JVM clients 

### Sample Use Case

You are the new line manager for a dev group. One of the applications you are now responsible for has been marked out as being slow.
This part of the batch process used to run in 5 minutes, now it takes over 20 minutes. You meet with the
team and they suspect the cause is a class written by Vicktor. Vicktor no longer works here and you get the feeling no one liked him
that much. The re-write of this class by Vicktor has been slated for 4 weeks of dev time by the previous manager. The class in question
is over 5000 lines, and the function in question is a nice 1500 lines.  So lets take a look and see what will discover...

We add a ```Signal``` to the "bad" method.

![alt text](https://github.com/rguadagno/French-Roast/blob/master/docs/editor.png "")

We then start the target application having already copied the French-Roast agent.dll to the target area and specified the address and port of the server.

![alt text](https://github.com/rguadagno/French-Roast/blob/master/docs/cmd_line_target.png "")

The Signals viewer shows us what happened with the badFunction. 10 is the number of times the function was invoked. The name of the thread is
indicated by [ main ].
![alt text](https://github.com/rguadagno/French-Roast/blob/master/docs/bad_func.png "")
So it appears the function in question is only invoked 10 times. You run this by the team and they say "yeah but it takes a long
time to run."  So we change the ```<ENTER>``` to a ```<TIMER>```
![alt text](https://github.com/rguadagno/French-Roast/blob/master/docs/editor_2.png "")

And re-kick the target application.
![alt text](https://github.com/rguadagno/French-Roast/blob/master/docs/timers.png "")

Now we see that the elapsed time is 45 seconds (for all 10 calls to the function ) for the "bad" method. This seems not so great but it also could be
worse as far as performance goes. But what else is going on? Let's take a look at some Hot Stacks and Methods in the Traffic window.

We set the sample rate to 10 milliseconds and re-kick the application again...
![alt text](https://github.com/rguadagno/French-Roast/blob/master/docs/traffic.png "")

You notice a hot method called ```doSomething()```, a member of the ```Parse``` class, that calls ```process()``` which obtains a monitor. Knowing locks(monitor) can affect performance we add a signal to ```process()``` by selecting that cell in the Traffic window then typing the ```s``` key. This populates the Editor with the descriptor of the method.
![alt text](https://github.com/rguadagno/French-Roast/blob/master/docs/editor_3.png "")

Then we re-kick the target application again.

![alt text](https://github.com/rguadagno/French-Roast/blob/master/docs/signals.png "")

Now we observe that ```process()``` (which acquires a lock) is called over 50,000 times with the exact same arguments. We stop the target application! Something is wrong here.
![alt text](https://github.com/rguadagno/French-Roast/blob/master/docs/with_details.png "")

Looking at the code we understand that the ```process()``` function is actually called 10 million times with the same argument values each time... this could have been cached once.

You meet with the team again asking about the ```process()``` method and they respond "oh that was written by Louie, the summer intern..."


### Technology

- C++
- Qt
- JVMTI
- JNI
- Java bytecode instrumentation
- Sockets
- multi-threading






  