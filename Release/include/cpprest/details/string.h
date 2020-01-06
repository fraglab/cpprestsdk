/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
*
* Commonly used string processing functions
*
* =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
****/
#pragma once

#ifdef CPPREST_CPP11_STRING
    #include <algorithm>
    #include <cctype>
    #include <locale>
#else
    #include <boost/algorithm/string.hpp>
    #include <boost/algorithm/string/find.hpp>
    #include <boost/algorithm/string/predicate.hpp>
#endif

namespace web
{
namespace lib
{

#ifdef CPPREST_CPP11_STRING
    inline bool iequals(const std::string& input, const std::string& test)
    {
        return input.compare(test) == 0;
    }

    inline bool icontains(const std::string& input, const std::string& test)
    {
        return input.find(test) != std::string::npos;
    }

    inline bool ilexicographical_compare(const std::string& left, const std::string& right)
    {
        return std::lexicographical_compare(left.begin(), left.end(), right.begin(), right.end());
    }

    inline bool ifind_first(const std::string& input, const std::string& test)
    {
        return input.find(test) != std::string::npos;
    }
#else
    using boost::icontains;
    using boost::iequals;
    using boost::ifind_first;
    using boost::ilexicographical_compare;
#endif

namespace algorithm
{
#ifdef CPPREST_CPP11_STRING
    inline void ltrim(std::string &s)
    {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) { return !std::isspace(ch); }));
    }
    
    inline void rtrim(std::string &s)
    {
        s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) { return !std::isspace(ch); }).base(), s.end());
    }
    
    inline void trim(std::string &s)
    {
        ltrim(s);
        rtrim(s);
    }
#else
    using boost::algorithm::trim;
#endif

}
}
}