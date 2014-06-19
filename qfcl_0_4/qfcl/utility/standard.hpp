/* qfcl/utility/standard.hpp
 *
 * Copyright (C) 2014 James Hirschorn <James.Hirschorn@gmail.com>
 *
 * Use, modification and distribution are subject to 
 * the BOOST Software License, Version 1.0.
 * (See accompanying file LICENSE.txt)
 */

#ifndef	QFCL_UTILITY_STANDARD_HPP
#define QFCL_UTILITY_STANDARD_HPP

/*! \file standard.hpp
	\brief Utilities for the standard namespace.

	\author James Hirschorn
	\date May 17, 2014
*/

#include <boost/mpl/identity.hpp>
#include <boost/utility/enable_if.hpp>

#include <qfcl/utility/type_traits.hpp>

namespace qfcl {

namespace traits {

namespace detail {
	template<typename T>
	static RT1 standard_test(typename T::standard_type const *);
	template<typename T>
	static RT2 standard_test(...);
}	// namespace detail

//! whether the type \c T satisfies the \c Standard concept
template<typename T>
struct is_standard
	: mpl::bool_<sizeof(detail::standard_test<T>(nullptr)) == 1>
{
};

}	// namespace traits


//! Returns the standard version of the <c>Model</c>, if it has one, otherwise it returns the <c>Model</c> itself.
template<typename Model, typename Enable = void>
struct standard_type 
	: boost::mpl::identity<Model>
{};

template<typename Model>
struct standard_type<Model, typename boost::enable_if<traits::is_standard<Model>>::type>
	: boost::mpl::identity<typename Model::standard_type>
{};

}	// namespace qfcl

#endif !QFCL_UTILITY_STANDARD_HPP	
