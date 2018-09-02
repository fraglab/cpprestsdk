// Copyright (C) https://github.com/sony/nmos-cpp

#include "stdafx.h"
#include "pplx/pplx_utils.h"

#if (defined(_MSC_VER) && (_MSC_VER >= 1800)) && !CPPREST_FORCE_PPLX

#include <agents.h>

namespace Concurrency
{
    pplx::task<void> complete_after(unsigned int milliseconds, const pplx::cancellation_token& token)
    {
        task_completion_event<void> tce;

        auto timer = std::make_shared<concurrency::timer<int>>(milliseconds, 0);
        auto callback = std::make_shared<concurrency::call<int>>([tce](int) { tce.set(); });

        timer->link_target(callback.get());
        timer->start();

        auto result = pplx::create_task(tce, token);

        if (token.is_cancelable())
        {
            auto registration = token.register_callback([timer, tce]
            {
                timer->stop();

                if (!tce._IsTriggered())
                {
                    tce._Cancel();
                }
            });

            result.then([token, registration](pplx::task<void>) { token.deregister_callback(registration); });
        }

        result.then([timer, callback]{});

        return result;
    }
}

#else

#include <boost/asio/basic_waitable_timer.hpp>
#include "pplx/threadpool.h"

namespace pplx
{
    pplx::task<void> complete_after(unsigned int milliseconds, const pplx::cancellation_token& token)
    {
        pplx::task_completion_event<void> tce;

        typedef boost::asio::basic_waitable_timer<std::chrono::steady_clock> steady_timer;

        auto timer = std::make_shared<steady_timer>(crossplat::threadpool::shared_instance().service());

        timer->expires_from_now(std::chrono::duration_cast<steady_timer::duration>(std::chrono::milliseconds(milliseconds)));
        timer->async_wait([tce](const boost::system::error_code& ec)
        {
            if (ec == boost::asio::error::operation_aborted)
            {
                if (!tce._IsTriggered())
                {
                    tce._Cancel();
                }
            }
            else
            {
                tce.set();
            }
        });

        auto result = pplx::create_task(tce, token);

        if (token.is_cancelable())
        {
            auto registration = token.register_callback([timer]
            {
                boost::system::error_code ignored;
                timer->cancel(ignored);
            });

            result.then([token, registration](pplx::task<void>) { token.deregister_callback(registration); });
        }

        result.then([timer]{});

        return result;
    }
}

#endif