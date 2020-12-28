#pragma once
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

#include <atomic>
#include <chrono>
#include <future>
#include <memory>
#include <thread>
#include <type_traits>
#include <unordered_map>
#include <vector>

struct Timer
{
  Timer()
    : m_Abort(false)
    , m_Done(false)
  {}
  ~Timer() { wait(); }

  /**
   * @brief Begin execution of an asynchronous timer and return.
   * @param interval time-out after interval.
   * @param oneShot fire once and be done(true) or repeat unless killed (false)
   * @param func function to call on kill/time out
   * @param ...args arguments to callback func
   * @return thread id
   *
   * @warning FIXME: each thread gets a copy of args. The original arguments
   * remain unchanged.
   */
  template<typename Rep, typename Period, typename F, typename... A>
  inline std::thread::id start(
    const std::chrono::duration<Rep, Period>& interval,
    bool oneShot,
    F&& func,
    A&&... args)
  {
    using namespace std;
    auto thr = thread(
      [this, interval, oneShot](decay_t<F>&& func,
                                decay_t<A>&&... args) {
        // wait here
        fut.get();
        // ready
        m_Abort.store(false);
        m_Done.store(false);
        // run
        do {
          this_thread::sleep_for(interval);
          func(args...);
        } while (!(oneShot || m_Abort.load()));
        // exit
        m_Done.store(true);
      },
      std::forward<F>(func),
      std::forward<A>(args)...);

    // get that thread's id.
    m_Tid = thr.get_id();
    // fire away
    m_Promise.set_value();
    thr.detach();
    // done
    return m_Tid;
  }

  /**
   * @brief Stop and kill timer.
   */
  inline void kill() noexcept
  {
    while (!m_Done.load())
      m_Abort.store(true);
  }

  /**
   * @brief Wait for timer to finish.
   */
  inline void wait() noexcept
  {
    while (!m_Done.load()) {
    }
  }

  /**
   * @return true: timer was killed or it timed out
   */
  inline bool finished() const noexcept { return m_Done.load(); }

private:
  std::atomic_bool m_Abort, m_Done;
  std::promise<void> m_Promise;
  std::future<void> fut = m_Promise.get_future();
  std::thread::id m_Tid;
};

struct TimerCollection
{
  /**
   * @brief Create a new timer.
   * @param interval time-out after interval.
   * @param oneShot fire once and be done(true) or repeat unless killed (false)
   * @param func function to call on kill/time out
   * @param ...args arguments to callback func
   * @return unique timer id
   */
  template<typename Rep, typename Period, typename F, typename... A>
  inline std::size_t create(const std::chrono::duration<Rep, Period>& interval,
                         bool oneShot,
                         F func,
                         A&&... args)
  {
    m_Timers.emplace_back(new Timer);
    auto m_Tid = m_Timers.back()->start(interval, oneShot, func, args...);
    auto timerId = std::hash<std::thread::id>{}(m_Tid);
    m_TimerIdToIndex[timerId] = m_Timers.size() - 1;
    return timerId;
  }

  /**
   * @brief Check if timerId is valid.
   * @param timerId: identifier
   * @return true if it exists, else false
   */
  inline bool exists(const std::size_t& timerId) const
  {
    auto indexLu = m_TimerIdToIndex.find(timerId);
    if (indexLu != m_TimerIdToIndex.end()) {
      if (m_Timers[indexLu->second]->finished())
        return false;
      else
        return true;
    }
    return false;
  }

  /**
   * @brief Kill a specific timer.
   * @param timerId: A valid timer id.
   */
  inline void kill(const std::size_t& timerId)
  {
    if (!exists(timerId))
      return;
    m_Timers[m_TimerIdToIndex[timerId]]->kill();
    m_TimerIdToIndex.erase(timerId);
  }

private:
  std::vector<std::unique_ptr<Timer>> m_Timers;
  std::unordered_map<std::size_t, std::size_t> m_TimerIdToIndex;
};