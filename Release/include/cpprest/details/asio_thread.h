/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
*
* common definitions for ASIO SSL (either boost or standalone)
*
* =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
****/
#pragma once

#ifdef ASIO_STANDALONE
    #include <asio/detail/thread.hpp>
#else
    #include <boost/asio/detail/thread.hpp>
#endif

namespace web
{
namespace lib
{
namespace asio
{
namespace detail
{

#ifdef ASIO_STANDALONE
    using ::asio::detail::thread;    
#else
    using boost::asio::detail::thread;
#endif

}
}
}
}