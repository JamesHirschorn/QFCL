/* qfcl/random/engine/qfcl_distribution_adaptor.hpp
 *
 * Copyright (C) 2012 James Hirschorn <James.Hirschorn@gmail.com>
 *
 * Use, modification and distribution are subject to 
 * the BOOST Software License, Version 1.0.
 * (See accompanying file LICENSE.txt)
 */

#ifndef QFCL_RANDOM_QFCL_DISTRIBUTION_ADAPTOR_HPP
#define QFCL_RANDOM_QFCL_DISTRIBUTION_ADAPTOR_HPP

/*! \file qfcl/random/engine/qfcl_distribution_adaptor.hpp
	\brief Adapts a foreign distribution into one satisfying QFCL standards.

	\author James Hirschorn
	\date February 12, 2014
*/

#include <boost/utility/enable_if.hpp>

#include "distributions.hpp"

namespace qfcl {

namespace random {

template<typename Distribution>//, typename ResultType>
struct qfcl_distribution_adaptor : Distribution
{
	// use perfect forwarding for constructors
	qfcl_distribution_adaptor() {}

	template<typename T1>
	explicit qfcl_distribution_adaptor(T1 && a1) 
		: Distribution( standard::forward<T1>(a1) ) {}

	template<typename T1, typename T2>
	qfcl_distribution_adaptor(T1 && a1, T2 && a2) 
		: Distribution( std::forward<T1>(a1), std::forward<T2>(a2) ) {}

	// Don't force Distribution to be copy assignable.
	//qfcl_distribution_adaptor<Distribution> & operator=(qfcl_distribution_adaptor<Distribution> const & other)
	//{
	//	Distribution::operator=(other);
	//	return *this;
	//}
	//typedef ResultType result_type;

	static const variate_method method; 
};

}	// namespace random

}	// namespace qfcl

#endif	QFCL_RANDOM_QFCL_DISTRIBUTION_ADAPTOR_HPP