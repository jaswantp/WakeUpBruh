# Yet another C++ Timer.
[![Actions Status](https://github.com/jaswantp/WakeUpBruh/workflows/Build%20and%20Test/badge.svg)](https://github.com/jaswantp/GeometricPredicates/actions)

Use it to wake up stuff in your _supposedly_ _purposeful_ C++ project!

- _Claims_: _versatile_, _asynchronous_, _barebones_, _header-only_, _no frills_ C++ timer exactly _like many other repos you'd find elsewhere_!

Jk, lol; don't use in production There's a serious flaw in this code.

#### Supported: 
- Callback on _time-out_ / _kill_
  - Regular functions w/ arbitrary arguments ([_Varaiadic template parameter packs_](https://eli.thegreenplace.net/2014/variadic-templates-in-c/))
  - C++ _Lambdas_
- Force _Kill_ timers.

#### Poor man's Documentation
``` c++
struct Timer: 
    1. Begin execution of an asynchronous timer. Does not block calling thread.
      start(IntervalT interval,
          bool oneShot,
          OnFinishFunc callback,
          Args... args) -> thread::id
    
    - interval: duration after which timer should 'timeout'
    - oneShot : true:- fire once and be done, false:- repeat unless killed
    - callback: function to call on kill/time out
    - args    : arguments passed to callback func
    - return  : timerId
    FIXME: each thread gets a copy of args. The original arguments remain unchanged.

    2. Stop and kill timer.
      kill() -> void

    3. Wait for timer to finish.
      wait() -> void

    4. Query status.
      finished() -> bool
    - return  : true if timer was killed/timed out, else false

struct TimerCollection: 
    1. Create an asynchronous timer and return a unique identifier
      create(IntervalT interval,
          bool oneShot,
          OnFinishFunc callback,
          Args... args) -> int
    - interval: duration after which timer should 'timeout'
    - oneShot : true:- fire once and be done, false:- repeat unless killed
    - callback: function to call on kill/time out
    - args    : arguments passed to callback func
    - return  : timerId

    2. Kill an existing timer.
      kill(int timerId) -> void
    - timerId: A valid timerId.
```
#### Modes
A. _OneShot_: Fire once and become a zombie upon time-out.
```c++
#include "Timer.hpp"

static void wakeUpCall(...){...}

TimerCollection ts;
ts.create(1000ms, true, wakeUpCall, ...)
t.create(1s, true, [...](...){...}, ...) // w/ lambda
...
```

B. _Repeating_: Fire and stay alive unless killed.
```c++
#include "Timer.hpp"

static void wakeUpCall(...){...}

TimerCollection ts;
ts.create(1000ms, false, wakeUpCall, ...)
ts.create(1s, false, [...](...){...}, ...) // w/ lambda
...
```

### Uninteresting demo

``` shell
$ ./wub

Timer: 1| Wake The Fuck Up Bruhh ._. fu

Timer: 0| Wake The Fuck Up Bruhh ._. fu

Timer: 1| Wake The Fuck Up Bruhh ._. du // <- bug: changes only thread local args f-->d

Timer: 2| Wake The Fuck Up Bruhh ._. fu
```

### Serious Flaw
- As timers are created, run and eventually killed, the internal vector of timers grows in size 
with total disregard for system memory. 