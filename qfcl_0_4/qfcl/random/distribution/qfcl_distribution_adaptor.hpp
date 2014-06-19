/* qfcl/random/distribution/qfcl_distribution_adaptor.hpp
 *
 * Copyright (C) 2014 James Hirschorn <James.Hirschorn@gmail.com>
 *
 * Use, modification and distribution are subject to 
 * the BOOST Software License, Version 1.0.
 * (See accompanying file LICENSE.txt)
 */

#ifndef QFCL_RANDOM_QFCL_DISTRIBUTION_ADAPTOR_HPP
#define QFCL_RANDOM_QFCL_DISTRIBUTION_ADAPTOR_HPP

/*! \file qfcl/random/distribution/qfcl_distribution_adaptor.hpp
	\brief Adapts a foreign distribution into one satisfying QFCL standards.

	\author James Hirschorn
	\date February 12, 2014
*/

#include <type_traits>

namespace qfcl {

namespace random {

// <p>Distribution</p> is any C++ standard distribution.
template<typename Distribution, typename Method, typename Standard = Distribution>
struct qfcl_distribution_adaptor : Distribution
{
	// use perfect forwarding for constructors
	qfcl_distribution_adaptor() {}

	template<typename T1>
	explicit qfcl_distribution_adaptor(T1 && a1) 
		: Distribution( std::forward<T1>(a1) ) {}

	template<typename T1, typename T2>
	qfcl_distribution_adaptor(T1 && a1, T2 && a2) 
		: Distribution( std::forward<T1>(a1), std::forward<T2>(a2) ) {}

	typedef Method		method;
	//! standard version of the distribution
	typedef Standard	standard_type;
};

}	// namespace random

}	// namespace qfcl

#endif	!QFCL_RANDOM_QFCL_DISTRIBUTION_ADAPTOR_HPP