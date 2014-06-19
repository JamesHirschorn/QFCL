/* qfcl/random/generator/QuantLib_ZigguratRng.hpp
 *
 * Copyright (C) 2014 James Hirschorn <James.Hirschorn@gmail.com>
 *
 * Use, modification and distribution are subject to 
 * the BOOST Software License, Version 1.0.
 * (See accompanying file LICENSE.txt)
 */

#ifdef USE_QUANTLIB

#ifndef QFCL_RANDOM_QUANTLIB_ZIGGURAT_RNG_HPP
#define QFCL_RANDOM_QUANTLIB_ZIGGURAT_RNG_HPP

#include <ql/experimental/math/zigguratrng.hpp>

#include <qfcl/miscellaneous/strings.hpp>
#include <qfcl/random/distribution/distributions.hpp>
#include <qfcl/random/distribution/qfcl_distribution_adaptor.hpp>
#include <qfcl/random/engine/mersenne_twister.hpp>
#include <qfcl/random/engine/quantlib_mt19937.hpp>
#include <qfcl/random/generator/qfcl_variate_generator_adaptor.hpp>
#include <qfcl/utility/named_adaptor.hpp>
#include <qfcl/utility/tmp.hpp>

/*! \file qfcl/random/generator/QuantLib_ZigguratRng.hpp
	\brief The ZigguratRng generator from QuantLib.

	\note This is produces normal variates and the engine is a hard-coded MT19937.

	\author James Hirschorn
	\date June 2, 2014
*/

namespace qfcl {

namespace string {
typedef tmp::concatenate<Ziggurat_string, Rng_string>::type ZigguratRng_string;
}	// namespace string

namespace random {

namespace detail {
// fake distribution
struct QuantLib_normal_ziggurat_distribution
{
	typedef double result_type;
};
class QuantLib_normal_ziggurat 
	: public qfcl_distribution_adaptor<
		named_adaptor<
			QuantLib_normal_ziggurat_distribution
			, tmp::concatenate<string::QuantLib_prefix, string::Ziggurat_string>
			>
		, variate_method<ZIGGURAT>
		> 
{
};
}	// namespace detail

namespace standard {
class QuantLib_ZigguratRng
{
public:
	typedef detail::QuantLib_normal_ziggurat::result_type result_type;

	// By default QuantLib::ZigguratRng has seed = 0, in which case it obtains the 
	// actual seed from the system clock.
	QuantLib_ZigguratRng(unsigned long seed = mt19937::default_seed)
		: ql_gen(seed)
	{};

	result_type operator()()
	{
		return ql_gen.next().value;
	}
private:
	::QuantLib::ZigguratRng ql_gen;
};
}	// namespace standard

class QuantLib_ZigguratRng_unnamed
	: public standard::QuantLib_ZigguratRng
{
public:
	typedef variate_method<ZIGGURAT> method;
};

class QuantLib_ZigguratRng
	: public QuantLib_ZigguratRng_unnamed
{
public:
	typedef tmp::concatenate<
		  string::QuantLib_string
		, string::field_operator
		, string::ZigguratRng_string
		>::type name;
};

#if 0
typedef qfcl_variate_generator_adaptor<standard::QuantLib_ZigguratRng, QuantLib_mt19937, detail::QuantLib_normal_ziggurat>
	QuantLib_ZigguratRng;
#endif

}}	// namespace qfcl::random

#endif !QFCL_RANDOM_QUANTLIB_ZIGGURAT_RNG_HPP

#endif	USE_QUANTLIB
