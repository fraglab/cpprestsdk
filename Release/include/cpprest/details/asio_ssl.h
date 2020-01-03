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
    #include <asio/ssl.hpp>
    #include <asio/ssl/error.hpp>
#else
    #include <boost/asio/ssl.hpp>
    #include <boost/asio/ssl/error.hpp>
#endif

namespace web
{
namespace lib
{
namespace asio
{
namespace ssl
{

#ifdef ASIO_STANDALONE
    using namespace ::asio::ssl;    
#else
    using namespace boost::asio::ssl;
#endif

}
}
}
}