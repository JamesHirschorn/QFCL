/* qfcl/random/distribution/QuantLib_to_standard_variate_generator_adaptor.hpp
 *
 * Copyright (C) 2014 James Hirschorn <James.Hirschorn@gmail.com>
 *
 * Use, modification and distribution are subject to 
 * the BOOST Software License, Version 1.0.
 * (See accompanying file LICENSE.txt)
 */

#ifdef USE_QUANTLIB

#ifndef QFCL_RANDOM_DISTRIBUTION_QUANTLIB_TO_STANDARD_VARIATE_GENERATOR_ADAPTOR_HPP
#define QFCL_RANDOM_DISTRIBUTION_QUANTLIB_TO_STANDARD_VARIATE_GENERATOR_ADAPTOR_HPP

/*! \file qfcl/random/distribution/QuantLib_to_standard_variate_generator_adaptor.hpp
	\brief QuantLib variate generator to standard variate generator adaptor. 

	Converts a QuantLib variate generator into a standard variate generator.

	\author James Hirschorn
	\date May 12, 2014
*/

#include <functional>
#include <limits>

#include <qfcl/random/distribution/distributions.hpp>
#include <qfcl/random/distribution/qfcl_distribution_adaptor.hpp>
#include <qfcl/random/distribution/uniform_0in_1in.hpp>
#include <qfcl/random/generator/QuantLib_variate_generator_distribution_adaptor.hpp>

namespace qfcl {
namespace random {
/*! \brief Converts a QuantLib variate generator to a standard variate generator.
	\tparam Generator a QuantLib variate generator
	\tparam Distribution the distribution of the variates produced by <c>Generator</c> (optional)
*/
template<typename Generator, typename Engine, typename Distribution>
class QuantLib_to_standard_variate_generator_adaptor
{
public:
	typedef Engine					engine_type;
	typedef Distribution												distribution_type;
	typedef typename Generator::urng_type								QuantLib_uniform01_type;
	typedef typename Generator::sample_type::value_type					result_type;

	QuantLib_to_standard_variate_generator_adaptor(
		engine_type const& e = engine_type(),
		distribution_type const& d = distribution_type())
		: _gen(&_uniform01_rng), _uniform01_rng(e)
	{};

	result_type operator()()
	{
		return _gen.next().value;
	}
private:
	Generator _gen;
	QuantLib_uniform01_type _uniform01_rng;
};

}}	// namespace qfcl::random
#endif	!QFCL_RANDOM_DISTRIBUTION_QUANTLIB_TO_STANDARD_VARIATE_GENERATOR_ADAPTOR_HPP

#endif	USE_QUANTLIB
