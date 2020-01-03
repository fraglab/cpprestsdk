/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
*
* common definitions for ASIO (either boost or standalone)
*
* =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
****/
#pragma once

#ifdef ASIO_STANDALONE
    #include <asio.hpp>
    #include <asio/steady_timer.hpp>

#define LIB_ASIO_VERSION ASIO_VERSION
#else
    #include <boost/asio.hpp>
    #include <boost/asio/steady_timer.hpp>  
    #include <boost/system/error_code.hpp>

#define LIB_ASIO_VERSION BOOST_ASIO_VERSION
#endif

namespace web
{
namespace lib
{
namespace asio
{

#ifdef ASIO_STANDALONE
    using namespace ::asio;

    // Here we assume that we will be using std::error_code with standalone
    // Asio. This is probably a good assumption, but it is possible in rare
    // cases that local Asio versions would be used.
    using std::errc;

    inline asio::chrono::seconds seconds(long duration)
    {
        return asio::chrono::seconds(duration);
    }
#else
    using namespace boost::asio;

    using boost::system::error_code;
    namespace errc = boost::system::errc;

    using steady_timer = boost::asio::deadline_timer;
        
    inline boost::posix_time::time_duration seconds(long duration)
    {
        return boost::posix_time::seconds(duration);
    }
#endif

}
}
}