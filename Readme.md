# Yet another C++ Timer.
[![Actions Status](https://github.com/jaswantp/WakeUpBruh/workflows/Build%20and%20Test/badge.svg)](https://github.com/jaswantp/GeometricPredicates/actions)

_Claims_: _versatile_, _asynchronous_, _barebones_, _header-only_, _no frills_ C++ timer exactly _like many other repos you'd find elsewhere_!

Use this to wake up stuff in your _supposedly_ _purposeful_ C++ project!

Jk, lol; don't use in production There's a serious flaw in this code.

#### Supported: 
- Callback on _time-out_ / _kill_
  - Regular functions w/ arbitrary arguments ([_Varaiadic template parameter packs_](https://eli.thegreenplace.net/2014/variadic-templates-in-c/))
  - C++ _Lambdas_
- Force _Kill_ timers.

#### Poor man's Documentation
``` c++
struct Timer: 
    1. Fire an asynchronous timer and return a unique identifier
      add(IntervalT interval,
          bool oneShot,
          OnFinishFunc callback,
          Args... args) -> int
    - interval: duration after which timer should 'timeout'
    - oneShot : fire once and be done(true) or repeat unless killed (false)
    - callback: function to call on kill/time out
    - args    : arguments passed to callback func
    - return  : timerId

    2. Kill an existing timer.
      kill(int timerId) -> void
    - timerId: A valid timerId.

    3. When a timer has timed-out but not yet killed (oneShot)
      isZombie(int timerId) -> bool
    - timerId: A valid timer id.
    - return  : true if zombie, else false

    4. When a timer is killed (oneShot/repeating)
      isStale(int timerId) -> bool
    - timerId: A valid timerId.
    - return  : true if stale, else false
```
#### Modes
A. _OneShot_: Fire once and become a zombie upon time-out.
```c++
#include "Timer.hpp"

static void wakeUpCall(...){...}

Timer t;
t.add(1000ms, true, wakeUpCall, ...)
t.add(1s, true, [...](...){...}, ...) // w/ lambda
...
```

B. _Repeating_: Fire and stay alive unless killed.
```c++
#include "Timer.hpp"

static void wakeUpCall(...){...}

Timer t;
t.add(1000ms, false, wakeUpCall, ...)
t.add(1s, false, [...](...){...}, ...) // w/ lambda
...
```

### Uninteresting demo

``` shell
$ ./wub
Set : 0 (6000ms)
Set : 1 (4000ms)
Set : 2 (10000ms)
Init: 1 (4000ms)
Run : 1 (4000ms)
Init: 2 (10000ms)
Run : 2 (10000ms)
Init: 0 (6000ms)
Run : 0 (6000ms)
Finished 1

 Timer: 1| Wake The Fuck Up Bruhh ._. fu
Run : 1 (4000ms)
Finished 0

 Timer: 0| Wake The Fuck Up Bruhh ._. fu
Killed 1

 Timer: 1| Wake The Fuck Up Bruhh ._. fu
Finished 2

 Timer: 2| Wake The Fuck Up Bruhh ._. fu
```

### Flaw
- As timers are added, run and eventually killed/zombified, the internal vector of threads grows in size 
with total disregard for system memory. 
- Eventually, the container exceeds `std::vector::max_size()`. Ideally, do garbage-collection; erase `stale`/`zombie` timers. Plz do it yourself :)
