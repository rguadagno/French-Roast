## French-Roast
A Java application profiler + bytecode instrumentation API written in C++ using Qt.

### Motivation
Your are a line manager responsible for the applications your team develops. But having limited time one cannot be
intimate with all of the details such as which method is invoked the most and by who etc... So just use French-Roast to profile your applications and discover what is really going on...

### Features
- Hot Stack discovery including monitor indicators
- Hot Method discovery
- Jammed (lock contention) Stack viewer
- Signals (byte code instrumentation JNI callbacks) for any method on entry and exit
  - method arguments and stack traces per signal
  - class instance variable values per signal
- Timers provide elapsed execution for any method
- Signal Editor with wildcard method selection
- Loaded classes viewer (displays as classes are loaded)
- Remote profiling (only agent dll or so lib required on target machine(s)
- Multiple JVM clients can be profiled at same time

### Scenario

Your are the new line manager for a dev group. One of the applications you are now responsible for has been marked out as being slow.
Its one part of some batch process running in the past in around 5 minutes now takes over 20 minutes ... You meet with the
team and they suspect the cause is a class written by Vicktor. Victor no longer works here and you get the feeling no one liked him
that much. The re-write of this class by Vicktor has been slated for 4 weeks of dev time by the previous manager. The class in question
is over 5000 lines, and the function in question is a nice 1500 lines.  So lets take a look and see what will discover...

Add a Signal to the "bad" method.

![alt text](https://github.com/rguadagno/French-Roast/blob/master/docs/editor.png "")

We then start the target application. We have copied the French-Roast agent.dll to the target area and specified the address and port of the server.

![alt text](https://github.com/rguadagno/French-Roast/blob/master/docs/cmd_line_target.png "")

The Signals viewer shows us the what happened with the badFunction. 10 is the number of times the function was invoked. The name of the thread is
indicated by [ main ].
![alt text](https://github.com/rguadagno/French-Roast/blob/master/docs/bad_func.png "")
So it appears the function in question is only invoked 10 times. You run this by the team and they say yeah but it takes a long
time to run.  So we change the ```<ENTER>``` to a ```<TIMER>```
![alt text](https://github.com/rguadagno/French-Roast/blob/master/docs/editor_2.png "")



### Technology






  