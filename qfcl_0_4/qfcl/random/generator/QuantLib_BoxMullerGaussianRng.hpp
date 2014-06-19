/* qfcl/random/generator/QuantLib_BoxMullerGaussianRng.hpp
 *
 * Copyright (C) 2014 James Hirschorn <James.Hirschorn@gmail.com>
 *
 * Use, modification and distribution are subject to 
 * the BOOST Software License, Version 1.0.
 * (See accompanying file LICENSE.txt)
 */

#ifdef USE_QUANTLIB

#ifndef QFCL_RANDOM_QUANTLIB_BOXMULLERGAUSSIAN_RNG_HPP
#define QFCL_RANDOM_QUANTLIB_BOXMULLERGAUSSIAN_RNG_HPP

#include <ql/math/randomnumbers/boxmullergaussianrng.hpp>
#include <ql/math/randomnumbers/mt19937uniformrng.hpp>

#include <qfcl/miscellaneous/strings.hpp>
#include <qfcl/random/distribution/distributions.hpp>
#include <qfcl/random/distribution/qfcl_distribution_adaptor.hpp>
#include <qfcl/random/engine/detail/strings.hpp>
#include <qfcl/random/engine/quantlib_mt19937.hpp>
#include <qfcl/random/generator/qfcl_variate_generator_adaptor.hpp>
#include <qfcl/utility/named_adaptor.hpp>
#include <qfcl/utility/tmp.hpp>

/*! \file qfcl/random/generator/QuantLib_BoxMullerGaussianRng.hpp
	\brief The BoxMullerGaussianRng generator from QuantLib.

	\note The real type is hard-coded in QuantLib (normally as double).

	\author James Hirschorn
	\date June 6, 2014
*/

namespace qfcl {

namespace string {
typedef tmp::concatenate<Box_string, Muller_string, Gaussian_string, Rng_string>::type BoxMullerGaussianRng_string;
typedef tmp::concatenate<Mersenne_string, Twister_string, Uniform_string, Rng_string>::type MersenneTwisterUniformRng_string;
}	// namespace string

namespace random {

namespace detail {
// fake distribution
struct QuantLib_normal_box_muller_polar_distribution
{
	typedef ::QuantLib::Real result_type;
};
class QuantLib_normal_box_muller_polar 
	: public qfcl_distribution_adaptor<
		named_adaptor<
			QuantLib_normal_box_muller_polar_distribution
			, string::empty //tmp::concatenate<string::QuantLib_prefix, string::normal_box_muller_polar_name>
			>
		, variate_method<BOX_MULLER_POLAR>
		> 
{
};
}	// namespace detail

namespace standard {
template<typename UniformRng = ::QuantLib::MersenneTwisterUniformRng>
class QuantLib_BoxMullerGaussianRng
{
public:
	typedef detail::QuantLib_normal_box_muller_polar::result_type result_type;

	// By default QuantLib::BoxMullerGaussianRng has seed = 0, in which case it obtains the 
	// actual seed from the system clock.
	// We use the mt19937 default seed. Initialize with 0 to get QuantLib's default behaviour.
	QuantLib_BoxMullerGaussianRng(unsigned long seed = mt19937::default_seed)	
		: ql_gen(UniformRng(seed))
	{}
	
	QuantLib_BoxMullerGaussianRng(UniformRng const& rng)
		: ql_gen(rng)
	{};

	result_type operator()()
	{
		return ql_gen.next().value;
	}
private:
	::QuantLib::BoxMullerGaussianRng<UniformRng> ql_gen;
};
}	// namespace standard

//! add a name to QuantLib's MT generator
typedef named_adaptor<
	  ::QuantLib::MersenneTwisterUniformRng
	, tmp::concatenate<
		  string::QuantLib_string
		, string::field_operator
		, string::MersenneTwisterUniformRng_string
		>
	>
QuantLib_MersenneTwisterUniformRng;

template<typename UniformRng = QuantLib_MersenneTwisterUniformRng>
class QuantLib_BoxMullerGaussianRng
	: public standard::QuantLib_BoxMullerGaussianRng<UniformRng>
{
public:
	typedef typename tmp::concatenate<
		  string::QuantLib_string
		, string::field_operator
		, string::BoxMullerGaussianRng_string
		, typename names::template_typename<UniformRng>::type
		>::type name;
	typedef variate_method<BOX_MULLER_POLAR> method;
};

// worsened performance
#if 0
template<typename UniformRng = ::QuantLib::MersenneTwisterUniformRng>
class QuantLib_BoxMullerGaussianRng
	: public qfcl_variate_generator_adaptor<
			standard::QuantLib_BoxMullerGaussianRng<UniformRng>
		,	QuantLib_mt19937
		,	detail::QuantLib_normal_box_muller_polar
		>
{
typedef qfcl_variate_generator_adaptor<
			standard::QuantLib_BoxMullerGaussianRng<UniformRng>
		,	QuantLib_mt19937
		,	detail::QuantLib_normal_box_muller_polar
		> base_type;
public:
	QuantLib_BoxMullerGaussianRng(
		engine_type const& e = engine_type(),
		distribution_type const& d = distribution_type())
		: base_type(e.get_engine())
	{}
};
#endif

}}	// namespace qfcl::random

#endif !QFCL_RANDOM_QUANTLIB_BOXMULLERGAUSSIAN_RNG_HPP

#endif	USE_QUANTLIB
