#include "Timer.hpp"

#include <iostream>

struct SomeObject
{
  char a = 'f';
  char b = 'u';
};

static void
func(SomeObject so, int id)
{
  std::cout << "\n Timer: " << id << "| Wake The Fuck Up Bruhh ._."
            << " " << so.a << so.b << "\n";
}

int
main(int argc, char* argv[])
{
  using namespace std;
  SomeObject obj;
  int id = 0;
  Timer timer;
  auto tid0 = timer.add(6000ms, true, func, obj, 0); // one shot
  auto tid1 = timer.add(4000ms, false, func, obj, 1); // repeating
  auto tid2 = timer.add(10s, true, [&](int _id) {
    std::cout << "\n Timer: " << _id << "| Wake The Fuck Up Bruhh ._."
              << " " << obj.a << obj.b << "\n";
  }, 2); // one shot w/ lambda? Heck yea.
  
  this_thread::sleep_for(8000ms);
  timer.kill(tid1);
  return 0;
}