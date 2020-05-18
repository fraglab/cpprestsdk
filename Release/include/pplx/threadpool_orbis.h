/***
 * Copyright (C) Microsoft. All rights reserved.
 * Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
 *
 *
 * Simple Linux implementation of a static thread pool.
 *
 * For the latest on this and related APIs, please see: https://github.com/Microsoft/cpprestsdk
 *
 * =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
 ***/
#pragma once

#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <stdexcept>

#include "cpprest/details/cpprest_compat.h"

namespace crossplat
{
    class threadpool_impl {
    public:
        threadpool_impl(size_t);
        ~threadpool_impl();

        // add new work item to the pool
        template<class F, class... Args>
        void enqueue(F&& f, Args&&... args)
        {
            using return_type = typename std::result_of<F(Args...)>::type;

            auto task = std::make_shared< std::packaged_task<return_type()> >(
                std::bind(std::forward<F>(f), std::forward<Args>(args)...)
                );

            {
                std::unique_lock<std::mutex> lock(queue_mutex);

                // don't allow enqueueing after stopping the pool
                if (stop)
                    throw std::runtime_error("enqueue on stopped threadpool_impl");

                tasks.emplace([task]() { (*task)(); });
            }
            condition.notify_one();
        }

    private:
        // need to keep track of threads so we can join them
        std::vector< std::thread > workers;
        // the task queue
        std::queue< std::function<void()> > tasks;

        // synchronization
        std::mutex queue_mutex;
        std::condition_variable condition;
        bool stop;
    };

    class threadpool_orbis
    {
    public:
        threadpool_orbis(size_t num_threads) : m_service(num_threads) {};

        _ASYNCRTIMP static threadpool_orbis& shared_instance();
        _ASYNCRTIMP static std::unique_ptr<threadpool_orbis> __cdecl construct(size_t num_threads);

        virtual ~threadpool_orbis() = default;

        /// <summary>
        /// Initializes the cpprestsdk threadpool with a custom number of threads
        /// </summary>
        /// <remarks>
        /// This function allows an application (in their main function) to initialize the cpprestsdk
        /// threadpool with a custom threadcount. Libraries should avoid calling this function to avoid
        /// a diamond problem with multiple consumers attempting to customize the pool.
        /// </remarks>
        /// <exception cref="std::exception">Thrown if the threadpool has already been initialized</exception>
        static void initialize_with_threads(size_t num_threads);

        threadpool_impl& service() { return m_service; }

    protected:
        threadpool_impl m_service;
    };

} // namespace crossplat
