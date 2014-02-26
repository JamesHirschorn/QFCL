/* qfcl/random/distribution/normal_quantile.hpp
 *
 * Copyright (C) 2014 James Hirschorn <James.Hirschorn@gmail.com>
 *
 * Use, modification and distribution are subject to 
 * the BOOST Software License, Version 1.0.
 * (See accompanying file LICENSE.txt)
 */

#ifndef QFCL_RANDOM_DISTRIBUTION_NORMAL_QUANTILE_HPP
#define QFCL_RANDOM_DISTRIBUTION_NORMAL_QUANTILE_HPP

/*! \file qfcl/random/distribution/normal_quantile.hpp
	\brief Univariate normal PRNG, using the quantile.

	\author James Hirschorn
	\date February 14, 2014
*/

#include <qfcl/random/distribution/distributions.hpp>
#include <qfcl/random/distribution/qfcl_distribution_adaptor.hpp>
#include <qfcl/random/distribution/uniform_0in_1in.hpp>

namespace qfcl {
namespace random {
//! Version conforming to C++ standards
namespace std {

template<typename Quantile, typename RealType = double, typename U01_Dist = uniform_0in_1in<RealType>>
class normal_quantile 
{ 
public:
	typedef RealType result_type; 

	normal_quantile(RealType mean = 0.0, RealType sd = 1.0) : _q(mean, sd)
	{
	}

	template<typename Engine>
	result_type operator()(Engine & e)
	{
		return _q(_u01_dist(e)); 
	}
private:
	//RealType _mean;
	//RealType _sd;
	Quantile _q;
 	typedef U01_Dist uniform01_distribution_type;
	uniform01_distribution_type  _u01_dist;
};

}	// namespace std

template<typename Quantile, typename RealType, typename U01_Dist>
class normal_quantile 
	: public qfcl_distribution_adaptor<std::normal_quantile<typename Quantile, typename RealType, typename U01_Dist>>
{
public:
	static const variate_method method; 
};

template<typename Quantile, typename RealType, typename U01_Dist>
const variate_method normal_quantile<Quantile, RealType, U01_Dist>::method = QUANTILE;

}}	// namespace qfcl::random
#endif	!QFCL_RANDOM_DISTRIBUTION_NORMAL_QUANTILE_HPP
