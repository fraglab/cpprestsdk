#include "stdafx.h"

#if defined(ORBIS)

#include <pplx/pplxlinux.h>

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
            }

        }

        _PPLXIMP void linux_scheduler::schedule(TaskProc_t proc, _In_ void* param) {};

    }
}

#endif