/* qfcl/random/distribution/normal_QuantLib_box_muller.hpp
 *
 * Copyright (C) 2012 James Hirschorn <James.Hirschorn@gmail.com>
 *
 * Use, modification and distribution are subject to 
 * the BOOST Software License, Version 1.0.
 * (See accompanying file LICENSE.txt)
 */

#ifdef USE_QUANTLIB

#ifndef QFCL_RANDOM_DISTRIBUTION_NORMAL_QUANTLIB_BOX_MULLER_HPP
#define QFCL_RANDOM_DISTRIBUTION_NORMAL_QUANTLIB_BOX_MULLER_HPP

/*! \file qfcl/random/distribution/normal_QuantLib_box_muller.hpp
	\brief Univariate normal PRNG, using the Box-Muller method.

	Wraps QuantLib's normal variate generator.

	\author James Hirschorn
	\date February 11, 2014
*/

//#include <ql/math/randomnumbers/boxmullergaussianrng.hpp>
#include <qfcl/random/distribution/QuantLib/boxmullergaussianrng.hpp>

#include <qfcl/random/distribution/distributions.hpp>
#include <qfcl/random/distribution/qfcl_distribution_adaptor.hpp>
#include <qfcl/random/distribution/uniform_0in_1in.hpp>

namespace qfcl {
namespace random {
//! Version conforming to C++ standards
namespace standard {

//! Converts a standard distribution into a variate generator usable with QuantLib.
template<typename Distribution>
class QuantLib_variate_generator_adaptor
{
	typedef typename Distribution::result_type result_type;
public:
	typedef QuantLib::Sample<result_type> sample_type;

	QuantLib_variate_generator_adaptor()
	{
	}

	void operator=(QuantLib_variate_generator_adaptor<Distribution> const & other)
	{
		_dist = other._dist;
		_next = other._next;
	}

	typename sample_type next() const
	{
		return sample_type(_next, 1.0);
	}

	template<typename Engine>
	void operator()(Engine & e) 	
	{
		_next = _dist(e);
	}
private:
	Distribution _dist;
	result_type _next;
};

template<typename RealType = double, typename U01_Dist = uniform_0in_1in<RealType>>
class normal_QuantLib_box_muller
{
public:
	typedef RealType result_type;

	normal_QuantLib_box_muller()
		: _u01_gen(), _normal_gen(_u01_gen)
	{
	}
	// QuantLib::BoxMullerGaussianRng<U01_generator &> not copyable.
	//normal_QuantLib_box_muller & operator=(normal_QuantLib_box_muller<RealType, U01_Dist> const & other)
	//{
	//	_u01_gen = other._u01_gen;
	//	_normal_gen = other._normal_gen;
	//}

	template<typename Engine>
	result_type operator()(Engine & e)
	{
		_u01_gen(e);
		return _normal_gen.next().value;
	}
private:
	typedef QuantLib_variate_generator_adaptor<U01_Dist> U01_generator;
	typedef QuantLib::BoxMullerGaussianRng<U01_generator &> normal_generator;

	U01_generator _u01_gen;
	normal_generator _normal_gen;
};

}	// namespace standard

template<typename RealType = double, typename U01_Dist = uniform_0in_1in<RealType>>
class normal_QuantLib_box_muller
	: public qfcl_distribution_adaptor<standard::normal_QuantLib_box_muller<RealType, U01_Dist>>
{
	typedef qfcl_distribution_adaptor<standard::normal_QuantLib_box_muller<RealType, U01_Dist>> base_type;
public:
	// QuantLib design excludes this.
	//void operator=(normal_QuantLib_box_muller<RealType, U01_Dist> const & other)
	//{
	//	base_type::operator=(other);
	//}

	static const variate_method method; 
};

template<typename RealType, typename U01_Dist>
const variate_method normal_QuantLib_box_muller<RealType, U01_Dist>::method = BOX_MULLER;

}}	// namespace qfcl::random
#endif	!QFCL_RANDOM_DISTRIBUTION_NORMAL_QUANTLIB_BOX_MULLER_HPP

#endif	USE_QUANTLIB
