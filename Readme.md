# Yet another C++ Timer library.

_Claims_: _versatile_, _asynchronous_, _barebones_, _header-only_, _no frills_ C++ timer library.
 _like many other repos you'd find elsewhere_.

Use this to wake up stuff in your _supposedly_ _purposeful_ C++ project.

#### Supports: 
- Callback on _time-out_ / on _kill_
 - Regular functions w/ arbitrary arguments ([_Varaiadic template parameter packs_](https://eli.thegreenplace.net/2014/variadic-templates-in-c/))
 - C++ _Lambdas_ w/ arbitrary parameters.
- _Kill_ a timer

#### Poor Documentation
_struct Timer_: 

    1. Adds a timer and schedules for asynchronous run.
      add(IntervalT interval,
          bool oneShot,
          OnFinishFunc callback,
          Args... args) -> int
    - interval: duration after which timer should 'timeout'
    - oneShot : fire once and be done(true) or repeat unless killed (false)
    - callback: function to call on kill/time out
    - args    : function arguments to give callback func
    - return  : timer id

    2. Kill an existing timer.
      kill(int timerId) -> void
    - timerId: A valid timer id.

    3. When a timer has timed-out but not yet killed (oneShot)
      isZombie(int timerId) -> bool
    - timerId: A valid timer id.
    - return  : true if zombie, else false

    4. When a timer is killed (oneShot/repeating)
      isStale(int timerId) -> bool
    - timerId: A valid timer id.
    - return  : true if stale, else false

#### Modes
A. _OneShot_: Fire once and become a zombie upon time-out.
```
#include "Timer.hpp"

static void wakeUpCall(...){...}

Timer t;
t.add(1000, true, wakeUpCall, ...)
t.add(1000, true, [...](...){...}, ...) // w/ lambda
...
```

B. _Repeating_: Fire and stay alive unless killed.
```
#include "Timer.hpp"

static void wakeUpCall(...){...}

Timer t;
t.add(1000, false, wakeUpCall, ...)
t.add(1000, false, [...](...){...}, ...) // w/ lambda
...
```

### Useless demo

``` 
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