#include "stdafx.h"

#if defined(AZ_PLATFORM_PROVO)

#include <pplx/pplxlinux.h>
#include <pplx/pplxinterface.h>
#include <pplx/threadpool_orbis.h>

// PS4 specific headers
#include <kernel.h>

__uint64_t GetCurrentThreadId();

namespace pplx
{
    namespace details
    {
        namespace platform
        {
            _PPLXIMP long __cdecl GetCurrentThreadId()
            {
                return (long)(::GetCurrentThreadId());
            }

            void YieldExecution()
            {
                scePthreadYield();
            }
        }

        _PPLXIMP void linux_scheduler::schedule(TaskProc_t proc, _In_ void* param) 
        {
            crossplat::threadpool_orbis::shared_instance().service().enqueue(web::lib::bind(proc, param));
        }
    }
}

#endif