//
// system_executor.h
// ~~~~~~~~~~~~~~~~~
// System executor implementation.
//
// Copyright (c) 2014 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef EXECUTORS_EXPERIMENTAL_BITS_SYSTEM_EXECUTOR_H
#define EXECUTORS_EXPERIMENTAL_BITS_SYSTEM_EXECUTOR_H

#include <cassert>
#include <cstddef>
#include <type_traits>
#include <thread>
#include <vector>
#include <experimental/bits/scheduler.h>

namespace std {
namespace experimental {

class __system_executor_impl
{
public:
  __system_executor_impl()
  {
    _M_scheduler._Work_started();
    std::size_t __n = thread::hardware_concurrency();
    for (size_t __i = 0; __i < __n; ++__i)
      _M_threads.emplace_back([this](){ _M_scheduler._Run(); });
  }

  ~__system_executor_impl()
  {
    _M_scheduler._Work_finished();
    for (auto& __t: _M_threads)
      __t.join();
  }

  static __system_executor_impl& _Instance()
  {
    static __system_executor_impl __e;
    return __e;
  }

  template <class _F> void _Post(_F&& __f)
  {
    _M_scheduler._Post(forward<_F>(__f));
  }

private:
  __scheduler _M_scheduler;
  vector<thread> _M_threads;
};


template <class _Func>
inline void system_executor::post(_Func&& __f)
{
  __system_executor_impl::_Instance()._Post(forward<_Func>(__f));
}

template <class _Func>
void system_executor::dispatch(_Func&& __f)
{
  typename decay<_Func>::type tmp(forward<_Func>(__f));
  tmp();
}

inline system_executor::work system_executor::make_work()
{
  return work{};
}

} // namespace experimental
} // namespace std

#endif
