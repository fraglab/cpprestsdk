// Copyright (C) https://github.com/sony/nmos-cpp

#pragma once

#ifndef PPLX_PPLX_UTILS_H
#define PPLX_PPLX_UTILS_H

#include <chrono>
#include "pplx/pplxtasks.h"

#if (defined(_MSC_VER) && (_MSC_VER >= 1800)) && !CPPREST_FORCE_PPLX
namespace Concurrency
#else
namespace pplx
#endif
{
    /// <summary>
    ///     Creates a task that completes after a specified amount of time.
    /// </summary>
    /// <param name="milliseconds">
    ///     The number of milliseconds after which the task should complete.
    /// </param>
    /// <param name="token">
    ///     Cancellation token for cancellation of this operation.
    /// </param>
    /// <remarks>
    ///     Because the scheduler is cooperative in nature, the delay before the task completes could be longer than the specified amount of time.
    /// </remarks>
    pplx::task<void> complete_after(unsigned int milliseconds, const pplx::cancellation_token& token = pplx::cancellation_token::none());

    /// <summary>
    ///     Creates a task that completes after a specified amount of time.
    /// </summary>
    /// <param name="duration">
    ///     The amount of time (milliseconds and up) after which the task should complete.
    /// </param>
    /// <param name="token">
    ///     Cancellation token for cancellation of this operation.
    /// </param>
    /// <remarks>
    ///     Because the scheduler is cooperative in nature, the delay before the task completes could be longer than the specified amount of time.
    /// </remarks>
    template <typename Rep, typename Period>
    inline pplx::task<void> complete_after(const std::chrono::duration<Rep, Period>& duration, const pplx::cancellation_token& token = pplx::cancellation_token::none())
    {
        return duration > std::chrono::duration<Rep, Period>::zero()
            ? complete_after((unsigned int)std::chrono::duration_cast<std::chrono::milliseconds>(duration).count(), token)
            : pplx::task_from_result();
    }

    /// <summary>
    ///     Creates a task that completes at a specified time.
    /// </summary>
    /// <param name="time">
    ///     The time point at which the task should complete.
    /// </param>
    /// <param name="token">
    ///     Cancellation token for cancellation of this operation.
    /// </param>
    /// <remarks>
    ///     Because the scheduler is cooperative in nature, the time at which the task completes could be after the specified time.
    /// </remarks>
    template <typename Clock, typename Duration = typename Clock::duration>
    inline pplx::task<void> complete_at(const std::chrono::time_point<Clock, Duration>& time, const pplx::cancellation_token& token = pplx::cancellation_token::none())
    {
        return complete_after(time - Clock::now(), token);
    }
}

#endif