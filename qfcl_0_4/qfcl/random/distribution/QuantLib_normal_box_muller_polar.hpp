/* qfcl/random/distribution/QuantLib_normal_box_muller_polar.hpp
 *
 * Copyright (C) 2014 James Hirschorn <James.Hirschorn@gmail.com>
 *
 * Use, modification and distribution are subject to 
 * the BOOST Software License, Version 1.0.
 * (See accompanying file LICENSE.txt)
 */

#ifdef USE_QUANTLIB

#ifndef QFCL_RANDOM_DISTRIBUTION_QUANTLIB_NORMAL_BOX_MULLER_POLAR_HPP
#define QFCL_RANDOM_DISTRIBUTION_QUANTLIB_NORMAL_BOX_MULLER_POLAR_HPP

/*! \file qfcl/random/distribution/QuantLib_normal_box_muller_polar.hpp
	\brief Univariate normal PRNG, using the Box-Muller method.

	Wraps QuantLib's normal variate generator.

	\author James Hirschorn
	\date February 11, 2014
*/

#include <ql/math/randomnumbers/boxmullergaussianrng.hpp>

#include <qfcl/defines.hpp>
#include <qfcl/miscellaneous/strings.hpp>
#include <qfcl/random/distribution/distributions.hpp>
#include <qfcl/random/distribution/qfcl_distribution_adaptor.hpp>
#include <qfcl/random/distribution/uniform_0in_1in.hpp>
#include <qfcl/random/generator/QuantLib/boxmullergaussianrng.hpp>
#include <qfcl/random/generator/QuantLib_from_standard_variate_generator_adaptor.hpp>
#include <qfcl/random/generator/QuantLib_to_standard_variate_generator_adaptor.hpp>
#include <qfcl/random/generator/QuantLib_variate_generator_distribution_adaptor.hpp>
#include <qfcl/utility/named_adaptor.hpp>
#include <qfcl/utility/names.hpp>
#include <qfcl/utility/tmp.hpp>

namespace qfcl {
namespace random {
//! Version conforming to C++ standards
namespace standard {

template<typename RealType = double, typename U01_Dist = uniform_0in_1in<RealType>>
class QuantLib_normal_box_muller_polar
{
public:
	typedef RealType result_type;

	QuantLib_normal_box_muller_polar()
		: _u01_gen(), _normal_gen(&_u01_gen)
	{
	}

	//! copy ctor
	QuantLib_normal_box_muller_polar(QuantLib_normal_box_muller_polar const& other)
		: _u01_gen(other._u01_gen), _normal_gen(&_u01_gen)
	{
	}

	//! operator=
	QuantLib_normal_box_muller_polar & operator=(QuantLib_normal_box_muller_polar const & other)
	{
		_u01_gen = other._u01_gen;
		_normal_gen = &_;
		return *this;
	}

	template<typename Engine>
	result_type operator()(Engine & e)
	{
		_u01_gen(e);

		return _normal_gen.next().value;
	}
private:
	// QuantLib BoxMullerGaussianRng uses the rejection method, and thus there is no upper
	// bound to the random numbers consumed.
	typedef QuantLib_variate_generator_distribution_adaptor<U01_Dist, 0> U01_generator;
	typedef ::qfcl::QuantLib::BoxMullerGaussianRng<U01_generator const*, RealType> normal_generator;

	U01_generator _u01_gen;
protected:
	normal_generator _normal_gen;
};

}	// namespace standard

template<typename RealType = double, typename U01_Dist = uniform_0in_1in<RealType>>
class QuantLib_normal_box_muller_polar
	: public named_adaptor<
		  qfcl_distribution_adaptor<
			  standard::QuantLib_normal_box_muller_polar<RealType, typename U01_Dist::standard_type>
			, variate_method<BOX_MULLER_POLAR>
			>
		, tmp::concatenate<
			  string::QuantLib_prefix
			, string::normal_box_muller_polar_name
			, typename qfcl::names::template_typename<RealType>::type
			, typename qfcl::names::template_typename<U01_Dist>::type
			>
		>
{
//	typedef standard_type standard_type;
	typedef named_adaptor<
		  qfcl_distribution_adaptor<
			  standard::QuantLib_normal_box_muller_polar<RealType, typename U01_Dist::standard_type>
			, variate_method<BOX_MULLER_POLAR>
			>
		, tmp::concatenate<
			  string::QuantLib_prefix
			, string::normal_box_muller_polar_name
			, typename qfcl::names::template_typename<RealType>::type
			, typename qfcl::names::template_typename<U01_Dist>::type
			>
		> base_type;
	//! can bypass the named_adaptor
	//typedef qfcl_distribution_adaptor<
	//		  standard::QuantLib_normal_box_muller_polar<RealType, U01_Dist>
	//		, variate_method<BOX_MULLER_POLAR>
	//		> subbase_type;
public:
	using typename base_type::result_type;
	//using typename subbase_type::result_type;
	//typedef typename subbase_type::result_type result_type;
	//typedef typename standard::QuantLib_normal_box_muller_polar<RealType, U01_Dist>::result_type result_type;
	
	QuantLib_normal_box_muller_polar() {}

	//! more efficient method, but requires that an engine has previously been fed to <c>operator()</c>.
	result_type next()
	{
		return _normal_gen.next().value;
	}
};

/** variate generator */

namespace standard {
template<typename Engine, typename RealType, typename U01_Dist>
class variate_generator<Engine, QuantLib_normal_box_muller_polar<RealType, U01_Dist>>
	: public QuantLib_to_standard_variate_generator_adaptor<
			::qfcl::QuantLib::BoxMullerGaussianRng<
				QuantLib_from_standard_variate_generator_adaptor<variate_generator<Engine, U01_Dist>>
			  , RealType
			  >
			, Engine
			, QuantLib_normal_box_muller_polar<RealType, U01_Dist>
			>
{
	typedef QuantLib_to_standard_variate_generator_adaptor<
			::qfcl::QuantLib::BoxMullerGaussianRng<
				QuantLib_from_standard_variate_generator_adaptor<variate_generator<Engine, U01_Dist>>
			  , RealType
			  >
			, Engine
			, QuantLib_normal_box_muller_polar<RealType, U01_Dist>
			> base_type;
public:
	typedef Engine													engine_type;
	typedef QuantLib_normal_box_muller_polar<RealType, U01_Dist>	distribution_type;
	typedef typename distribution_type::result_type					result_type;

	//! ctor
	variate_generator(
		engine_type const& e = engine_type(), 
		distribution_type const& d = distribution_type())
		: base_type(e, d)
	{}
};
}	// namespace standard

template<typename Engine, typename RealType, typename U01_Dist>
class variate_generator<Engine, QuantLib_normal_box_muller_polar<RealType, U01_Dist>>
	: public qfcl_variate_generator_adaptor<
			standard::variate_generator<Engine, typename QuantLib_normal_box_muller_polar<RealType, U01_Dist>::standard_type>
		,	Engine
		,	QuantLib_normal_box_muller_polar<RealType, U01_Dist>
		>
{
	typedef qfcl_variate_generator_adaptor<
			standard::variate_generator<Engine, typename QuantLib_normal_box_muller_polar<RealType, U01_Dist>::standard_type>
		,	Engine
		,	QuantLib_normal_box_muller_polar<RealType, U01_Dist>
		> base_type;
public:
	variate_generator(
		Engine const& e = Engine(), 
		QuantLib_normal_box_muller_polar<RealType, U01_Dist> const& d = QuantLib_normal_box_muller_polar<RealType, U01_Dist>())
		: base_type(e, d)
	{}
};

}}	// namespace qfcl::random
#endif	!QFCL_RANDOM_DISTRIBUTION_QUANTLIB_NORMAL_BOX_MULLER_POLAR_HPP

#endif	USE_QUANTLIB
