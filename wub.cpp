#include "Timer.hpp"

#include <iostream>

struct SomeObject
{
  char a = 'f';
  char b = 'u';
};

static void
func(SomeObject& so, int id)
{
  std::cout << "\nTimer: " << id << "| Wake The Fuck Up Bruhh ._."
            << " " << so.a << so.b << "\n";
  so.a = 'd';
}

int
main(int argc, char* argv[])
{
  using namespace std;
  SomeObject obj;
  int id = 0;
  TimerCollection timers;
  auto tid0 = timers.create(6000ms, true, func, obj, 0); // one shot
  auto tid1 = timers.create(4000ms, false, func, obj, 1); // repeating
  auto tid2 = timers.create(
    10s,
    true,
    [&](int _id) {
    std::cout << "\nTimer: " << _id << "| Wake The Fuck Up Bruhh ._."
              << " " << obj.a << obj.b << "\n";
  }, 2); // one shot w/ lambda? Heck yea.
  
  this_thread::sleep_for(8000ms);
  timers.kill(tid1);
  return 0;
}