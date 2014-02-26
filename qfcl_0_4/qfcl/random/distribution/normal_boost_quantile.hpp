/* qfcl/random/distribution/normal_boost_quantile.hpp
 *
 * Copyright (C) 2014 James Hirschorn <James.Hirschorn@gmail.com>
 *
 * Use, modification and distribution are subject to 
 * the BOOST Software License, Version 1.0.
 * (See accompanying file LICENSE.txt)
 */

#ifndef QFCL_RANDOM_DISTRIBUTION_NORMAL_BOOST_QUANTILE_HPP
#define QFCL_RANDOM_DISTRIBUTION_NORMAL_BOOST_QUANTILE_HPP

/*! \file qfcl/random/distribution/normal_boost_quantile.hpp
	\brief Univariate normal PRNG, using boost's quantile function.

	\author James Hirschorn
	\date February 14, 2014
*/

#include <boost/math/distributions/normal.hpp>

#include <qfcl/random/distribution/distributions.hpp>
#include <qfcl/random/distribution/normal_quantile.hpp>
#include <qfcl/random/distribution/qfcl_distribution_adaptor.hpp>
#include <qfcl/random/distribution/uniform_0in_1in.hpp>

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

template<typename RealType = double, typename U01_Dist = uniform_0in_1in<RealType>>
class normal_boost_quantile 
	: public normal_quantile<detail::boost_quantile<RealType>, RealType, U01_Dist>
{ 
};

}	// namespace standard

template<typename RealType = double, typename U01_Dist = uniform_0in_1in<RealType>>
class normal_boost_quantile 
	: public qfcl_distribution_adaptor<standard::normal_boost_quantile<RealType, U01_Dist>>
{
public:
	// needed because of bug in MSVC compiler, or because of complex rule?
	static const variate_method method; 
};

template<typename RealType, typename U01_Dist>
const variate_method normal_boost_quantile<RealType, U01_Dist>::method = QUANTILE;

}}	// namespace qfcl::random
#endif	!QFCL_RANDOM_DISTRIBUTION_NORMAL_BOOST_QUANTILE_HPP
