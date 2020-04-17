#include <pplx/threadpool_orbis.h>

//#include <memory>
#include <functional>

namespace crossplat
{
    template<class T>
    struct uninitialized
    {
        ~uninitialized()
        {
            if (initialized)
            {
                storage.reset();
            }
        }

        std::shared_ptr<T> storage;

        bool initialized;
        uninitialized() CPPREST_NOEXCEPT : initialized(false) {}
        uninitialized(const uninitialized&) = delete;
        uninitialized& operator=(const uninitialized&) = delete;

        template<class... Args>
        void construct(Args&&... vals)
        {
            storage = std::make_shared<T>(std::forward<Args>(vals)...);
            initialized = true;
        }
    };

    std::pair<bool, threadpool_orbis*> initialize_shared_threadpool(size_t num_threads)
    {
        static uninitialized<threadpool_orbis> uninit_threadpool;
        bool initialized_this_time = false;
        static std::once_flag of;

        std::call_once(of, [num_threads, &initialized_this_time] {
            uninit_threadpool.construct(num_threads);
            initialized_this_time = true;
            });

        return { initialized_this_time, uninit_threadpool.storage.get() };
    }

    _ASYNCRTIMP threadpool_orbis& threadpool_orbis::shared_instance()
    {
        return *initialize_shared_threadpool(20).second;
    }

    _ASYNCRTIMP std::unique_ptr<threadpool_orbis> __cdecl threadpool_orbis::construct(size_t num_threads)
    {
        return std::unique_ptr<threadpool_orbis>(new threadpool_orbis(num_threads));
    }

    void threadpool_orbis::initialize_with_threads(size_t num_threads)
    {
        const auto result = initialize_shared_threadpool(num_threads);
        if (!result.first)
        {
            throw std::runtime_error("the cpprestsdk threadpool has already been initialized");
        }
    }

    // the constructor just launches some amount of workers
    inline threadpool_impl::threadpool_impl(size_t threads)
        : stop(false)
    {
        for (size_t i = 0; i < threads; ++i)
            workers.emplace_back("cpprest_thread",
                [this]
                {
                    for (;;)
                    {
                        std::function<void()> task;

                        {
                            std::unique_lock<std::mutex> lock(this->queue_mutex);
                            this->condition.wait(lock,
                                [this] { return this->stop || !this->tasks.empty(); });
                            if (this->stop && this->tasks.empty())
                                return;
                            task = std::move(this->tasks.front());
                            this->tasks.pop();
                        }

                        task();
                    }
                }
                );
    }



    // the destructor joins all threads
    inline threadpool_impl::~threadpool_impl()
    {
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            stop = true;
        }
        condition.notify_all();
        for (std::thread& worker : workers)
            worker.join();
    }
}