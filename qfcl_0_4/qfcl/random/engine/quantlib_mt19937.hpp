/* qfcl/random/engine/QuantLib_mt19937.hpp
 *
 * Copyright (C) 2014 James Hirschorn <James.Hirschorn@gmail.com>
 *
 * Use, modification and distribution are subject to 
 * the BOOST Software License, Version 1.0.
 * (See accompanying file LICENSE.txt)
 */

#ifdef USE_QUANTLIB

#ifndef QFCL_RANDOM_QUANTLIB_MT19937_HPP
#define QFCL_RANDOM_QUANTLIB_MT19937_HPP

/*! \file qfcl/random/engine/QuantLib_mt19937.hpp
	\brief Wraps the QuantLib MT generator to obtain one compliant with QFCL.

	\author James Hirschorn
	\date February 5, 2014
*/

#include <boost/mpl/string.hpp>

#include <ql/math/randomnumbers/mt19937uniformrng.hpp>

#include <qfcl/miscellaneous/strings.hpp>
#include <qfcl/random/engine/mersenne_twister.hpp>
#include <qfcl/random/engine/qfcl_engine_adapter.hpp>
#include <qfcl/utility/tmp.hpp>
#include <qfcl/utility/named_adaptor.hpp>

namespace qfcl {

namespace string {
typedef qfcl::tmp::concatenate<QuantLib_prefix, mt19937_name>::type QuantLib_mt19937_name;
}	// namespace string

namespace random {
	
/* QuantLib engine names */

//! \cond
namespace detail {

typedef named_adaptor<
	qfcl_engine_adapter<
			::QuantLib::MersenneTwisterUniformRng
		,	unsigned long
		,	unsigned long[624]
	>, string::QuantLib_mt19937_name> ql_mt19937;
}	// namespace detail
//! \endcond

/*! \ingroup random
	@{
*/

namespace standard {

class QuantLib_mt19937 : public detail::ql_mt19937
{
public:
	result_type operator()()
	{
		return e.nextInt32();
	}
};

}	// namespace standard

// named QFCL versions of QuantLib engines

typedef named_adaptor<standard::QuantLib_mt19937, string::QuantLib_mt19937_name> QuantLib_mt19937; 

//! @}

}	// namespace random

}	// namespace qfcl

#endif !QFCL_RANDOM_QUANTLIB_MT19937_HPP

#endif USE_QUANTLIB
