/* qfcl/random/distribution/normal_boost_quantile.hpp
 *
 * Copyright (C) 2014 James Hirschorn <James.Hirschorn@gmail.com>
 *
 * Use, modification and distribution are subject to 
 * the BOOST Software License, Version 1.0.
 * (See accompanying file LICENSE.txt)
 */

#ifndef QFCL_RANDOM_DISTRIBUTION_BOOST_NORMAL_QUANTILE_HPP
#define QFCL_RANDOM_DISTRIBUTION_BOOST_NORMAL_QUANTILE_HPP

/*! \file qfcl/random/distribution/boost_normal_quantile.hpp
	\brief Univariate normal PRNG, using boost's quantile function.

	\author James Hirschorn
	\date February 14, 2014
*/

#include <boost/math/distributions/normal.hpp>

#include <qfcl/miscellaneous/strings.hpp>
#include <qfcl/random/distribution/distributions.hpp>
#include <qfcl/random/distribution/normal_quantile.hpp>
#include <qfcl/random/distribution/qfcl_distribution_adaptor.hpp>
#include <qfcl/random/distribution/uniform_0ex_1ex.hpp>
#include <qfcl/utility/tmp.hpp>
#include <qfcl/utility/named_adaptor.hpp>

namespace qfcl {
namespace random {

namespace detail {

template<typename RealType>
struct boost_quantile
{
	boost_quantile(RealType mean, RealType sd)
		: normal_distribution(mean, sd)
	{
	};

	RealType operator()(RealType x) const
	{
		return boost::math::quantile(normal_distribution, x);
	}
private:
	boost::math::normal_distribution<RealType> normal_distribution;
};

}	// namespace detail

//! Version conforming to C++ standards
namespace standard {

template<typename RealType = double, typename U01_Dist = uniform_0ex_1ex<RealType>>
class boost_normal_quantile 
	: public standard::normal_quantile<qfcl::random::detail::boost_quantile<RealType>, RealType, U01_Dist>
{ 
};

}	// namespace standard

template<typename RealType = double, typename U01_Dist = uniform_0ex_1ex<RealType>>
class boost_normal_quantile 
	: public named_adaptor<
			qfcl_distribution_adaptor<standard::boost_normal_quantile<RealType, U01_Dist>
		  , variate_method<QUANTILE>
		  >
		, typename tmp::concatenate<
			string::boost_prefix
		  , string::normal_quantile_name
		  , typename names::template_typename<RealType>::type
		  , typename names::template_typename<U01_Dist>::type
		  >::type
		>
{
};

}}	// namespace qfcl::random
#endif	!QFCL_RANDOM_DISTRIBUTION_BOOST_NORMAL_QUANTILE_HPP
