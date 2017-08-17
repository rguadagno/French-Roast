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

### Scenarios


Add a Signal to the "bad" method.

![alt text](https://github.com/rguadagno/French-Roast/blob/master/docs/editor.png "")
### Technology






  