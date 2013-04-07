/// Provides thin wrapper for Utils::Manip::to_stream and LOG_VAR() macro to simplify log output

//          Copyright Dmitriy Maksimov 2009 - 2013.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)


#ifndef TO_STREAM_HELPER_HPP
#define TO_STREAM_HELPER_HPP

#include "to_stream.hpp"


namespace Utils
{
    namespace Manip
    {
        namespace details
        {
            template <typename T>
            struct ToStreamWrapper
            {
                const T& data;

                ToStreamWrapper(const T& var)
                    : data(var)
                {
                }
            };

            template <typename T>
            const ToStreamWrapper<T> ToStreamHelper(const T& var)
            {
                return ToStreamWrapper<T>(var);
            }

            template <typename CharT, typename Traits, typename T>
            std::basic_ostream<CharT, Traits>& operator<<(std::basic_ostream<CharT, Traits> &strm, const ToStreamWrapper<T>& ts)
            {
                return ::Utils::Manip::to_stream(strm, ts.data);
            }
        }	// namespace details
    }	// namespace Manip
}	// namespace Utils

#define LOG_VAR(VAR) #VAR "<" << ::Utils::Manip::details::ToStreamHelper(VAR) << "> "

#endif // TO_STREAM_HELPER_HPP
