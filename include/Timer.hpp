#ifndef Timer_h__
#define Timer_h__

/**
 * Copyright © 2020 Jaswant Panchumarti <jaswantphysx@gmail.com>
 * This work is free. You can redistribute it and/or modify it
 * under the terms of the Do What The Fuck You Want To Public License,
 * Version 2, as published by Sam Hocevar.
 * See the COPYING file for more details.
 */

/**
 * This program is free software. It comes without any warranty,
 * to the extent permitted by applicable law.You can redistribute it and / or
 * modify it under the terms of the Do What The Fuck You Want To Public
 * License, Version 2, as published by Sam Hocevar.See http://www.wtfpl.net/ for
 * more details.
 */

#include <chrono>
#include <condition_variable>
#include <future>
#ifdef WUB_LOG_TIMER
#include <iostream>
#endif // WUB_LOG_TIMER
#include <map>
#include <mutex>
#include <unordered_set>
#include <vector>

struct Timer
{
  template<typename Rep,
           typename Period,
           typename OnFinishFunc,
           typename... Args>
  inline int add(const std::chrono::duration<Rep, Period>& interval,
                 bool oneShot,
                 OnFinishFunc callback,
                 Args... args)
  {
    using namespace std::chrono;
    const auto msec = duration_cast<milliseconds>(interval);
    return add(msec.count(), oneShot, callback, args...);
  }
  /**
   * @brief Instantiate a timer to run for said interval.
   * Calls a callback when killed/timed out.
   * @param interval: duration (ms) after which timer should 'timeout'
   * @param oneShot : fire once and be done or repeat after 'timeout'
   * @param callback: function to call on kill/time out
   * @param ...args : function arguments to give callback func
   * @return timer id.
   */
  template<typename IntervalT,
           typename = std::enable_if_t<std::is_arithmetic_v<IntervalT>>,
           typename OnFinishFunc,
           typename... Args>
  inline int add(IntervalT interval,
                 bool oneShot,
                 OnFinishFunc callback,
                 Args... args)
  {
    int timerId = 0;
    {
      using namespace std;
      scoped_lock lk(m_Mtx);
      timerId = static_cast<int>(m_Threads.size());
      m_Threads.emplace_back(
        async(launch::async,
              &Timer::run<IntervalT, OnFinishFunc, Args...>,
              this,
              timerId,
              interval,
              oneShot,
              callback,
              forward<Args>(args)...));
#ifdef WUB_LOG_TIMER
      cout << "Set : " << timerId << " (" << interval << "ms)\n";
#endif // WUB_LOG_TIMER
    }
    return timerId;
  }

  /**
   * @brief Kill an existing timer.
   * @param timerId: A valid timer id.
   */
  inline void kill(int timerId)
  {
    if ((timerId >= m_Threads.size()) ||
        (m_Stale.find(timerId) != m_Stale.end()))
      return;

    while (this->m_Threads[timerId].wait_for(std::chrono::seconds(0)) !=
           std::future_status::ready) {
      std::scoped_lock lk(m_Mtx);
      m_Terminate[timerId] = true;
      m_Stale.insert(timerId);
    }
    m_Cv.notify_all();
  }

  /**
   * @brief When a timer has timed-out but not yet killed (oneShot)
   * @param timerId A valid timerId
   * @return true if zombie, else false
   */
  inline bool isZombie(int timerId) const
  {
    return m_Zombies.find(timerId) != m_Zombies.end();
  }

  /**
   * @brief When a timer is killed (oneShot/repeating)
   * @param timerId A valid timerId
   * @return true if stale, else false
   */
  inline bool isStale(int timerId) const
  {
    return m_Stale.find(timerId) != m_Stale.end();
  }

private:
  std::condition_variable m_Cv;
  std::map<int, bool> m_Terminate;
  std::mutex m_Mtx;
  std::unordered_set<int> m_Zombies, m_Stale;
  std::vector<std::future<void>> m_Threads;

  template<typename IntervalT, typename OnFinishFunc, typename... Args>
  inline void run(int timerId,
                  IntervalT interval,
                  bool oneShot,
                  OnFinishFunc callback,
                  Args... args)
  {
    using namespace std;
    static_assert(is_arithmetic_v<IntervalT>);

    auto time = chrono::duration<IntervalT, milli>(interval);
    {
      scoped_lock lk(m_Mtx);
      m_Terminate[timerId] = false;
#ifdef WUB_LOG_TIMER
      cout << "Init: " << timerId << " (" << interval << "ms)\n";
#endif // WUB_LOG_TIMER
    }

    do {
      unique_lock<mutex> lk(m_Mtx);
#ifdef WUB_LOG_TIMER
      cout << "Run : " << timerId << " (" << interval << "ms)\n";
#endif // WUB_LOG_TIMER
      bool killed =
        m_Cv.wait_for(lk, time, [&]() { return m_Terminate[timerId]; });
#ifdef WUB_LOG_TIMER
      cout << (killed ? "Killed " : "Finished ") << timerId << "\n";
#endif // WUB_LOG_TIMER
      callback(args...);
    } while (!(oneShot || m_Terminate[timerId]));

    if (m_Stale.find(timerId) == m_Stale.end())
      if (oneShot)
        m_Zombies.insert(timerId);
  }
};
#endif // Timer_h__