/* qfcl/random/distribution/boost_uniform_01.hpp
 *
 * Copyright (C) 2012 James Hirschorn <James.Hirschorn@gmail.com>
 *
 * Use, modification and distribution are subject to 
 * the BOOST Software License, Version 1.0.
 * (See accompanying file LICENSE.txt)
 */

#ifndef QFCL_RANDOM_BOOST_UNIFORM_01_HPP
#define QFCL_RANDOM_BOOST_UNIFORM_01_HPP

/*! \file qfcl/random/distribution/boost_uniform_01.hpp
	\brief Wraps the boost::uniform_01 distribution for use with QFCL.

	Makes it additionally satisfy the Named Concept, and provide information
	on what method is used.

	\author James Hirschorn
	\date February 12, 2014
*/

#include <boost/mpl/string.hpp>
#include <boost/random/uniform_01.hpp>

#include <qfcl/random/distribution/distributions.hpp>
#include <qfcl/random/distribution/qfcl_distribution_adaptor.hpp>
#include <qfcl/utility/tmp.hpp>
#include <qfcl/utility/named_adapter.hpp>

namespace qfcl {
namespace random {

template<typename RealType = double>
struct boost_uniform_01 
	: qfcl_distribution_adaptor<boost::uniform_01<RealType>> 
{
	// shouldn't need this, but MSVC compiler seems to be broken
	static const variate_method method;
private:
	typedef qfcl_distribution_adaptor<boost::uniform_01<RealType>> base_type;
};

template<typename RealType>
const variate_method boost_uniform_01<RealType>::method = QUANTILE;
//const variate_method boost_uniform_01<RealType>::base_type::method = QUANTILE;
//const variate_method qfcl_distribution_adaptor<boost::uniform_01<RealType, double>>::method = QUANTILE;

}}	// namespace qfcl::random

#endif !QFCL_RANDOM_BOOST_UNIFORM_01_HPP
