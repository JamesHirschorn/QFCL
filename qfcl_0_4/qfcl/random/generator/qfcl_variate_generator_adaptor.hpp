/* qfcl/random/qfcl_variate_generator_adaptor.hpp
 *
 * Copyright (C) 2014 James Hirschorn <James.Hirschorn@gmail.com>
 *
 * Use, modification and distribution are subject to 
 * the BOOST Software License, Version 1.0.
 * (See accompanying file LICENSE.txt)
 */

#ifndef QFCL_RANDOM_QFCL_VARIATE_GENERATOR_ADAPTOR_HPP
#define QFCL_RANDOM_QFCL_VARIATE_GENERATOR_ADAPTOR_HPP

/*! \file qfcl/random/qfcl_variate_generator_adaptor.hpp
	\brief Adapts a foreign variate generator into one satisfying QFCL standards.

	\note Requires that the underlying distribution and engine satisfy QFCL standards.

	\author James Hirschorn
	\date May 4, 2014
*/

#include <type_traits>

#include <qfcl/miscellaneous/strings.hpp>
#include <qfcl/utility/named_adaptor.hpp>
#include <qfcl/utility/names.hpp>
#include <qfcl/utility/tmp.hpp>

namespace qfcl {

namespace random {

namespace detail {
typedef tmp::concatenate<
		string::prefix<string::variate_string, '_'>::type
	,	string::generator_string>::type 
variate_generator_string;
}	// namespace detail

// <p>Generator</p> is any C++ variate generator.
template<typename Generator, typename Engine, typename Distribution>
struct qfcl_variate_generator_adaptor 
	: qfcl::named_adaptor<
			Generator
		,	typename tmp::concatenate<
				detail::variate_generator_string
			,	typename names::template_typename<Engine>::type
			,	typename names::template_typename<Distribution>::type
			>::type
		>
{
private:
	typedef qfcl::named_adaptor<
			Generator
		,	typename tmp::concatenate<
				detail::variate_generator_string
			,	typename names::template_typename<Engine>::type
			,	typename names::template_typename<Distribution>::type
			>::type
		>
	base_type;
public:
	//! redefine the engine and distribution types
	typedef Engine								engine_type;
	typedef Distribution						distribution_type;
	typedef typename Distribution::result_type	result_type;
	typedef typename Distribution::method		method;

	// use perfect forwarding for constructors
	qfcl_variate_generator_adaptor() : base_type() {}

	template<typename T1>
	explicit qfcl_variate_generator_adaptor(T1 && a1) 
		: base_type( std::forward<T1>(a1) ) {}

	template<typename T1, typename T2>
	qfcl_variate_generator_adaptor(T1 && a1, T2 && a2) 
		: base_type( std::forward<T1>(a1), std::forward<T2>(a2) ) {}
};

}	// namespace random

}	// namespace qfcl

#endif	!QFCL_RANDOM_QFCL_VARIATE_GENERATOR_ADAPTOR_HPP