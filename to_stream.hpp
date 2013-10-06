/// Convert [almost] any type to a string

//          Copyright Dmitriy Maksimov 2009 - 2013.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)


#ifndef TO_STREAM_HPP
#define TO_STREAM_HPP

// STL
#include <ostream>
#include <utility>
#include <string>
#include <queue>
#include <stack>

// Boost
#include <boost/config.hpp>

#include <boost/type_traits/is_same.hpp>
#include <boost/type_traits/ice.hpp>

#include <boost/utility/enable_if.hpp>

#include <boost/mpl/if.hpp>
#include <boost/mpl/bool.hpp>
#include <boost/mpl/and.hpp>
#include <boost/mpl/not.hpp>
#include <boost/mpl/has_xxx.hpp>

#include <boost/optional.hpp>


namespace Utils
{
    namespace Manip
    {
        template <typename CharT, typename Traits, typename T>
        std::basic_ostream<CharT, Traits>& to_stream(std::basic_ostream<CharT, Traits> &strm, const T& var);
    }	// namespace Manip

    namespace details
    {
        template<typename T>
        class HasToStringMethod
        {
            template<typename U, std::string (U::*)() const> struct SFINAE {};
            template<typename U> static boost::type_traits::yes_type Impl(SFINAE<U, &U::to_string>*);
            template<typename U> static boost::type_traits::no_type Impl(...);
        public:
            BOOST_STATIC_CONSTANT(bool, value = ( sizeof(Impl<T>(0)) == sizeof(boost::type_traits::yes_type) ) );
        };

        template <typename CharT, typename Traits, typename T>
        class HasToStreamMethod
        {
            template<typename U, std::basic_ostream<CharT, Traits>& (U::*)(std::basic_ostream<CharT, Traits>&) const> struct SFINAE {};
            template<typename U> static boost::type_traits::yes_type Impl(SFINAE<U, &U::to_stream>*);
            template<typename U> static boost::type_traits::no_type Impl(...);
        public:
            BOOST_STATIC_CONSTANT(bool, value = ( sizeof(Impl<T>(0)) == sizeof(boost::type_traits::yes_type) ) );
        };

        struct AnyType
        {
            template <class T>
            AnyType(T)
            {
            }
        };

        template <typename CharT, typename Traits>
        boost::type_traits::no_type operator<<(std::basic_ostream<CharT, Traits> &, AnyType);

        template <class CharT, typename Traits, class T>
        class IsOutStreamable
        {
            static std::basic_ostream<CharT, Traits>& GetStrm();
            static const T& GetT();
            static boost::type_traits::no_type Impl(boost::type_traits::no_type);
            static boost::type_traits::yes_type Impl(...);
        public:
            BOOST_STATIC_CONSTANT(bool, value = ( sizeof(Impl(GetStrm() << GetT())) == sizeof(boost::type_traits::yes_type) ) );
        };

        BOOST_MPL_HAS_XXX_TRAIT_DEF(const_iterator)
        BOOST_MPL_HAS_XXX_TRAIT_DEF(value_type)

        template<typename T>
        struct IsContainer
        {
            BOOST_STATIC_CONSTANT(bool, value = ( boost::mpl::and_<
                has_const_iterator<T>,
                has_value_type<T>,
                boost::mpl::not_<boost::is_same<T, std::string> >,
                boost::mpl::not_<boost::is_same<T, std::wstring> >
            >::value )
            );
        };

        template <typename CharT, typename Traits, typename T>
        struct UseOutputOperator
        {
            BOOST_STATIC_CONSTANT(bool, value = ( IsOutStreamable<CharT, Traits, T>::value ) );
        };

        template <typename CharT, typename Traits, typename T>
        struct UseToStream
        {
            BOOST_STATIC_CONSTANT(bool, value = ( boost::type_traits::ice_and<
                boost::type_traits::ice_not<UseOutputOperator<CharT, Traits, T>::value>::value,
                HasToStreamMethod<CharT, Traits, T>::value
            >::value )
            );
        };

        template <typename CharT, typename Traits, typename T>
        struct UseToString
        {
            BOOST_STATIC_CONSTANT(bool, value = ( boost::type_traits::ice_and<
                boost::type_traits::ice_not<UseOutputOperator<CharT, Traits, T>::value>::value,
                boost::type_traits::ice_not<UseToStream<CharT, Traits, T>::value>::value,
                HasToStringMethod<T>::value
            >::value )
            );
        };

        template <typename CharT, typename Traits, typename T>
        typename boost::enable_if_c<UseOutputOperator<CharT, Traits, T>::value, std::basic_ostream<CharT, Traits>&>::type
        to_stream(std::basic_ostream<CharT, Traits>& strm, const T& val)
        {
            return ( strm << val );
        }

        template <typename CharT, typename Traits, typename T>
        typename boost::enable_if_c<UseToStream<CharT, Traits, T>::value, std::basic_ostream<CharT, Traits>&>::type
        to_stream(std::basic_ostream<CharT, Traits>& strm, const T& val)
        {
            return val.to_stream(strm);
        }

        template <typename CharT, typename Traits, typename T>
        typename boost::enable_if_c<UseToString<CharT, Traits, T>::value, std::basic_ostream<CharT, Traits>&>::type
        to_stream(std::basic_ostream<CharT, Traits>& strm, const T& val)
        {
            return ( strm << val.to_string() );
        }

        // === bool
        template <typename CharT, typename Traits>
        std::basic_ostream<CharT, Traits>& to_stream(std::basic_ostream<CharT, Traits>& strm, const bool& val)
        {
            return ( strm << ( val ? "true" : "false" ) );
        }

        template <typename CharT, typename Traits>
        std::basic_ostream<CharT, Traits>& to_stream(std::basic_ostream<CharT, Traits>& strm, const unsigned char& val)
        {
            return ( strm << static_cast<int>(val) );
        }

		// === std::pair
        template <typename CharT, typename Traits, typename T, typename U>
        std::basic_ostream<CharT, Traits>& to_stream(std::basic_ostream<CharT, Traits>& strm, const std::pair<T, U>& val)
        {
            strm << '(';
            ::Utils::Manip::to_stream(strm, val.first);
            strm << ", ";
            ::Utils::Manip::to_stream(strm, val.second);
            strm << ')';

            return strm;
        }

        template <typename CharT, typename Traits, typename T>
        typename boost::enable_if<IsContainer<T>, std::basic_ostream<CharT, Traits>&>::type
        to_stream(std::basic_ostream<CharT, Traits>& strm, const T& val)
        {
            strm << '[' << val.size() << "](";

            if ( !val.empty() )
            {
                typename T::const_iterator it = val.begin();
                ::Utils::Manip::to_stream(strm, *it++);
                for (; it != val.end(); ++it)
                {
                    strm << ", ";
                    ::Utils::Manip::to_stream(strm, *it);
                }
            }

            strm << ')';

            return strm;
        }

        template <typename CharT, typename Traits, typename T>
        std::basic_ostream<CharT, Traits>& to_stream(std::basic_ostream<CharT, Traits>& strm, const std::queue<T>& val)
        {
            std::queue<T> tmp(val);
            strm << '[' << tmp.size() << "](";

            if ( !val.empty() )
            {
                ::Utils::Manip::to_stream(strm, tmp.front());
                tmp.pop();

                while ( !tmp.empty() )
                {
                    strm << ", ";
                    ::Utils::Manip::to_stream(strm, tmp.front());
                    tmp.pop();
                }
            }

            strm << ')';

            return strm;
        }

        template <typename CharT, typename Traits, typename T>
        std::basic_ostream<CharT, Traits>& OutputStackOrPriorityQueue(std::basic_ostream<CharT, Traits>& strm, const T& val)
        {
            T tmp(val);
            strm << '[' << tmp.size() << "](";

            if ( !val.empty() )
            {
                ::Utils::Manip::to_stream(strm, tmp.top());
                tmp.pop();

                while ( !tmp.empty() )
                {
                    strm << ", ";
                    ::Utils::Manip::to_stream(strm, tmp.top());
                    tmp.pop();
                }
            }

            strm << ')';

            return strm;
        }

        template <typename CharT, typename Traits, typename T>
        std::basic_ostream<CharT, Traits>& to_stream(std::basic_ostream<CharT, Traits>& strm, const std::priority_queue<T>& val)
        {
            return OutputStackOrPriorityQueue(strm, val);
        }

        template <typename CharT, typename Traits, typename T>
        std::basic_ostream<CharT, Traits>& to_stream(std::basic_ostream<CharT, Traits>& strm, const std::stack<T>& val)
        {
            return OutputStackOrPriorityQueue(strm, val);
        }

        template <typename CharT, typename Traits, typename T>
        std::basic_ostream<CharT, Traits>& to_stream(std::basic_ostream<CharT, Traits>& strm, const boost::optional<T>& val)
        {
            if ( val )
            {
                return ::Utils::Manip::to_stream(strm, val.get());
            }
            else
            {
                return ( strm << "--" );
            }
        }

        template <typename CharT, typename Traits, typename T, size_t N>
        std::basic_ostream<CharT, Traits>& to_stream(std::basic_ostream<CharT, Traits>& strm, const T (&val)[N])
        {
            strm << '[' << N << "](";

            ::Utils::Manip::to_stream(strm, val[0]);
            for (size_t i = 1; i < N; ++i)
            {
                strm << ", ";
                ::Utils::Manip::to_stream(strm, val[i]);
            }

            strm << ')';

            return strm;
        }
    }	// namespace details

    namespace Manip
    {
        template <typename CharT, typename Traits, typename T>
        std::basic_ostream<CharT, Traits>& to_stream(std::basic_ostream<CharT, Traits> &strm, const T& var)
        {
            return details::to_stream(strm, var);
        }
    }	// namespace Manip
}	// namespace Utils

#endif	// TO_STREAM_HPP
