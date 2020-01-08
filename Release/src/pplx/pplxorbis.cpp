#include "stdafx.h"

#if defined(AZ_PLATFORM_PROVO)

#include <pplx/pplxlinux.h>
#include <pplx/pplxinterface.h>

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

        _PPLXIMP void linux_scheduler::schedule(TaskProc_t proc, _In_ void* param) {}
    }

    _PPLXIMP std::shared_ptr<scheduler_interface> _pplx_cdecl get_ambient_scheduler()
    {
        return nullptr;
    }
}

#endif