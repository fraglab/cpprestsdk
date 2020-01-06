/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
*
* C++11 std::bind support
*
* =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
****/
#pragma once

// If we're on Visual Studio 2010 or higher and haven't explicitly disabled
// the use of C++11 functional header then prefer it to boost.
#if defined(_MSC_VER) && _MSC_VER >= 1600 && !defined CPPREST_NO_CPP11_FUNCTIONAL
#ifndef CPPREST_CPP11_FUNCTIONAL
#define CPPREST_CPP11_FUNCTIONAL
#endif
#endif

#ifdef CPPREST_CPP11_FUNCTIONAL
    #include <functional>
#else
    #include <boost/bind.hpp>
    #include <boost/function.hpp>
#endif

namespace web
{
namespace lib
{

#ifdef CPPREST_CPP11_FUNCTIONAL
    using std::function;
    using std::bind;
#else
    using boost::function;
    using boost::bind;
#endif

}
}