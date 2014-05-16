/* qfcl/random/distribution/QuantLib_from_standard_variate_generator_adaptor.hpp
 *
 * Copyright (C) 2014 James Hirschorn <James.Hirschorn@gmail.com>
 *
 * Use, modification and distribution are subject to 
 * the BOOST Software License, Version 1.0.
 * (See accompanying file LICENSE.txt)
 */

#ifdef USE_QUANTLIB

#ifndef QFCL_RANDOM_DISTRIBUTION_QUANTLIB_FROM_STANDARD_VARIATE_GENERATOR_ADAPTOR_HPP
#define QFCL_RANDOM_DISTRIBUTION_QUANTLIB_FROM_STANDARD_VARIATE_GENERATOR_ADAPTOR_HPP

/*! \file qfcl/random/distribution/QuantLib_from_standard_variate_generator_adaptor.hpp
	\brief QuantLib variate generator from standard variate generator adaptor. 

	Converts a standard variate generator into a QuantLib variate generator.

	\author James Hirschorn
	\date May 14, 2014
*/

#include <type_traits>

#include <ql/methods/montecarlo/sample.hpp>

namespace qfcl {
namespace random {
/*! \brief Converts a standard variate generator to a Quantlib variate generator.
	\tparam Generator a standard variate generator
*/
template<typename Generator>
class QuantLib_from_standard_variate_generator_adaptor
{
public:
	typedef typename Generator::result_type	result_type;
	typedef ::QuantLib::Sample<result_type>	sample_type;

	//! default ctor
	QuantLib_from_standard_variate_generator_adaptor() {}

	//! ctors
	template<typename T1>
	explicit QuantLib_from_standard_variate_generator_adaptor(T1 && a1) 
		: _std_gen( std::forward<T1>(a1) ) {}

	template<typename T1, typename T2>
	QuantLib_from_standard_variate_generator_adaptor(T1 && a1, T2 && a2) 
		: _std_gen( std::forward<T1>(a1), std::forward<T2>(a2) ) {}

	sample_type next() const
	{
		return sample_type(nextReal(), 1.0);
	}
	result_type nextReal() const
	{
		return _std_gen();
	}
private:
	mutable Generator _std_gen;
};

}}	// namespace qfcl::random
#endif	!QFCL_RANDOM_DISTRIBUTION_QUANTLIB_FROM_STANDARD_VARIATE_GENERATOR_ADAPTOR_HPP

#endif	USE_QUANTLIB
