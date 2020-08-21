#pragma once

#include <atomic>
#include <chrono>
#include <functional>
#include <thread>

using namespace std::chrono_literals;

using _system_time_point = std::chrono::time_point<std::chrono::system_clock>;

template <class _rep, class _period>
using _duration = std::chrono::duration<_rep, _period>;
// TODO: c++20: _any_duration (концепт)

using _milliseconds = std::chrono::milliseconds;

struct _timer
{
	std::atomic_bool active = false;

	template <class _f, class _rep, class _period, class... _params>
	void start(_duration<_rep, _period> delay, _f f, _params... par)
	{
		active = true;
		std::thread([=]() {
			while (true)
			{
				if (!active) return;
				std::this_thread::sleep_for(delay);
				if (!active) return;
				f(par...);
			}
		}).detach();
	}

	void stop() { active = false; }
};

// struct TimeRequest {
//  std::atomic_bool waiting = false;
//  operator bool() { return waiting; }
//  void join() {
//    while (waiting) std::this_thread::sleep_for(1ms);
//  }
//  ~TimeRequest() { join(); }
//
//  template <class TDur, class TPeriod>
//  void require(std::chrono::duration<TDur, TPeriod> duration) {
//    if (waiting) return;
//    waiting = true;
//    std::thread([this, duration]() {
//      std::this_thread::sleep_for(duration);
//      this->waiting = false;
//    }).detach();
//  }
//};

namespace test
{
	template <class F, class... TParams>
	auto duration(F f, TParams... par)
	{
		auto t1 = std::chrono::high_resolution_clock::now();
		f(par...);
		auto t2 = std::chrono::high_resolution_clock::now();
		return (t2 - t1);
	}
	template <class F, class... TParams>
	auto duration_avg(F f, int count, TParams... par)
	{
		auto t1 = std::chrono::high_resolution_clock::now();
		for (int i = 0; i < count; ++i) f(par...);
		auto t2 = std::chrono::high_resolution_clock::now();
		return (t2 - t1) / count;
	}
	template <class F, class... TParams>
	auto duration_min(F f, int count, TParams... par)
	{
		auto tmn = std::chrono::nanoseconds::max();
		for (int i = 0; i < count; ++i)
		{
			auto t = duration(f, par...);
			if (t < tmn) tmn = t;
		}
		return tmn;
	}
} // namespace test
