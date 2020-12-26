#include "Timer.hpp"
#include "testing.h"

static void
dummyFunc()
{}

bool
testIsZombie(int& id, const Timer& timer, const char* message)
{
  bool result = timer.isZombie(id);
  IS_TRUE(result, message);
  return result;
}

bool
testIsNotZombie(int& id, const Timer& timer, const char* message)
{
  bool result = !timer.isZombie(id);
  IS_TRUE(result, message);
  return result;
}

bool
testIsStale(int& id, const Timer& timer, const char* message)
{
  bool result = timer.isStale(id);
  IS_TRUE(result, message);
  return result;
}

bool
testIsNotStale(int& id, const Timer& timer, const char* message)
{
  bool result = !timer.isStale(id);
  IS_TRUE(result, message);
  return result;
}

int
main(int argc, char* argv[])
{
  using namespace std;
  Timer timer;
  int oneshot = timer.add(1000ms, true, dummyFunc);
  int repeating = timer.add(1s, false, []() {});

  bool result =
    testIsNotZombie(oneshot, timer, "Timer upon creation is not a zombie.");
  result &= testIsNotStale(oneshot, timer, "Timer upon creation is not stale.");

  this_thread::sleep_for(2000ms);
  result &=
    testIsZombie(oneshot, timer, "Timer post time-out becomes a zombie.");
  result &=
    testIsNotStale(repeating, timer, "Timer post time-out is not stale .");

  timer.kill(repeating);
  result &= testIsStale(repeating, timer, "Timer becomes stale when killed.");
  result &=
    testIsNotZombie(repeating, timer, "Timer is not zombie when killed.");
  return result ? EXIT_SUCCESS : EXIT_FAILURE;
}
