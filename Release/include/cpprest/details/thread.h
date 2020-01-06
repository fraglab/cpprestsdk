/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
*
* C++11 thread support
*
* =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
****/
#pragma once

// If we're on Visual Studio 2013 or higher and haven't explicitly disabled
// the use of C++11 thread header then prefer it to boost.
#if defined(_MSC_VER) && _MSC_VER >= 1800 && !defined CPPREST_NO_CPP11_THREAD
#ifndef CPPREST_CPP11_THREAD
#define CPPREST_CPP11_THREAD
#endif
#endif

#ifdef CPPREST_CPP11_THREAD 
    #include <thread>
    #include <mutex>
    #include <condition_variable>
#else
    #include <boost/thread.hpp>
    #include <boost/thread/mutex.hpp>
    #include <boost/thread/condition_variable.hpp>
#endif

namespace web
{
namespace lib
{

#ifdef CPPREST_CPP11_THREAD
    using std::mutex;
    using std::lock_guard;
    using std::thread;
    using std::unique_lock;
    using std::condition_variable;
#else
    using boost::mutex;
    using boost::lock_guard;
    using boost::thread;
    using boost::unique_lock;
    using boost::condition_variable;
#endif

}
}