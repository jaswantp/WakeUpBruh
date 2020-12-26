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
  SomeObject obj;
  int id = 0;
  Timer timer;
  auto tid0 = timer.add(6000, true, func, obj, 0);
  auto tid1 = timer.add(4000, false, func, obj, 1);
  {
    using namespace std;
    std::this_thread::sleep_for(8000ms);
  }
  timer.kill(tid1);
  return 0;
}