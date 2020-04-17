/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
*
* read_untill definitions for ASIO (either boost or standalone)
*
* =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
****/
#pragma once

#ifdef ASIO_STANDALONE
    #include <asio/read_until.hpp>
#else
    #include <boost/asio/read_until.hpp>
#endif

namespace web
{
namespace lib
{
namespace asio
{

#ifdef ASIO_STANDALONE
    using ::asio::async_read_until;
    using ::asio::async_read;
    using ::asio::async_write;
#else
    using boost::asio::async_read_until;
    using boost::asio::async_read;
    using boost::asio::async_write;
#endif

}
}
}