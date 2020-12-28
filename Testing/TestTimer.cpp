#include "Timer.hpp"
#include "testing.h"

static void
dummyFunc()
{}

bool
testExists(std::size_t& id, const TimerCollection& timers, const char* message)
{
  bool result = timers.exists(id);
  IS_TRUE(result, message);
  return result;
}

bool
testDoesNotExist(std::size_t& id,
                 const TimerCollection& timers,
                 const char* message)
{
  bool result = !timers.exists(id);
  IS_TRUE(result, message);
  return result;
}

int
main(int argc, char* argv[])
{
  using namespace std;
  TimerCollection timers;
  auto oneshotSmol = timers.create(10us, true, dummyFunc);
  auto repeatable = timers.create(10us, false, []() {});
  auto oneshotBig = timers.create(10us, true, []() {});

  bool result =
    testExists(oneshotSmol, timers, "exists upon creation. (oneshotSmol)");
  result &=
    testExists(repeatable, timers, "exists upon creation. (repeatable)");

  timers.kill(oneshotBig);
  result &= testDoesNotExist(
    oneshotBig, timers, "when killed, it ceases to exist. (oneshotBig)");

  this_thread::sleep_for(20us);
  result &= testDoesNotExist(
    oneshotSmol, timers, "post time-out, it ceases to exist. (oneshotSmol)");

  timers.kill(repeatable);
  result &= testDoesNotExist(
    repeatable, timers, "when killed, it ceases to exist. (repeatable)");
  return result ? EXIT_SUCCESS : EXIT_FAILURE;
}
