/* qfcl/random/distribution/normal_QuantLib_box_muller_polar.hpp
 *
 * Copyright (C) 2014 James Hirschorn <James.Hirschorn@gmail.com>
 *
 * Use, modification and distribution are subject to 
 * the BOOST Software License, Version 1.0.
 * (See accompanying file LICENSE.txt)
 */

#ifdef USE_QUANTLIB

#ifndef QFCL_RANDOM_DISTRIBUTION_NORMAL_QUANTLIB_BOX_MULLER_POLAR_HPP
#define QFCL_RANDOM_DISTRIBUTION_NORMAL_QUANTLIB_BOX_MULLER_POLAR_HPP

/*! \file qfcl/random/distribution/normal_QuantLib_box_muller_polar.hpp
	\brief Univariate normal PRNG, using the Box-Muller method.

	Wraps QuantLib's normal variate generator.

	\author James Hirschorn
	\date February 11, 2014
*/

//#include <ql/math/randomnumbers/boxmullergaussianrng.hpp>
#include <qfcl/random/distribution/QuantLib/boxmullergaussianrng.hpp>

#include <qfcl/random/distribution/distributions.hpp>
//#include <qfcl/random/distribution/qfcl_distribution_adaptor.hpp>
#include <qfcl/random/distribution/uniform_0in_1in.hpp>
#include <qfcl/random/distribution/QuantLib_variate_generator_adaptor.hpp>

namespace qfcl {
namespace random {
//! Version conforming to C++ standards
namespace standard {

template<typename RealType = double, typename U01_Dist = uniform_0in_1in<RealType>>
class normal_QuantLib_box_muller_polar
{
public:
	typedef RealType result_type;

	normal_QuantLib_box_muller_polar()
		: _u01_gen(), _normal_gen(_u01_gen)
	{
	}
	// QuantLib::BoxMullerGaussianRng<U01_generator &> not copyable.
	//normal_QuantLib_box_muller_polar & operator=(normal_QuantLib_box_muller_polar<RealType, U01_Dist> const & other)
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
	// QuantLib BoxMullerGaussianRng uses the rejection method, and thus there is no upper
	// bound to the random numbers consumed.
	typedef QuantLib_variate_generator_adaptor<U01_Dist, 0> U01_generator;
	typedef QuantLib::BoxMullerGaussianRng<U01_generator &> normal_generator;

	U01_generator _u01_gen;
protected:
	normal_generator _normal_gen;
};

}	// namespace standard

template<typename RealType = double, typename U01_Dist = uniform_0in_1in<RealType>>
class normal_QuantLib_box_muller_polar
	: public qfcl_distribution_adaptor<standard::normal_QuantLib_box_muller_polar<RealType, U01_Dist>>
{
	typedef qfcl_distribution_adaptor<standard::normal_QuantLib_box_muller_polar<RealType, U01_Dist>> base_type;
public:
	using typename base_type::result_type;

	//! more efficient method, but requires that an engine has previously been fed to <c>operator()</c>.
	result_type next()
	{
		return _normal_gen.next().value;
	}
	// QuantLib design excludes this.
	//void operator=(normal_QuantLib_box_muller_polar<RealType, U01_Dist> const & other)
	//{
	//	base_type::operator=(other);
	//}

	static const variate_method method; 
};

template<typename RealType, typename U01_Dist>
const variate_method normal_QuantLib_box_muller_polar<RealType, U01_Dist>::method = BOX_MULLER_POLAR;

}}	// namespace qfcl::random
#endif	!QFCL_RANDOM_DISTRIBUTION_NORMAL_QUANTLIB_BOX_MULLER_POLAR_HPP

#endif	USE_QUANTLIB
