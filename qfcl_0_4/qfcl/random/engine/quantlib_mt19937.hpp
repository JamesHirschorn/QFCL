/* qfcl/random/engine/quantlib_mt19937.hpp
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

/*! \file qfcl/random/engine/quantlib_mt19937.hpp
	\brief Wraps the QuantLib MT generator to obtain one compliant with QFCL.

	\author James Hirschorn
	\date February 5, 2014
*/

#include <boost/mpl/string.hpp>

#include <ql/math/randomnumbers/mt19937uniformrng.hpp>

#include <qfcl/utility/tmp.hpp>
#include <qfcl/utility/named_adapter.hpp>

#include "mersenne_twister.hpp"
#include "standards_adapter.hpp"

namespace qfcl {

namespace random {
	
/* QuantLib engine names */

//! \cond
namespace detail {

//! alias
namespace mpl = boost::mpl;

typedef mpl::string<'Q', 'u', 'a', 'n', 't'>::type _Quant_string;
typedef mpl::string<'L', 'i', 'b'>::type _Lib_string;
typedef qfcl::tmp::concatenate<_Quant_string, _Lib_string, mpl::string<'-'>::type>::type _QuantLib_prefix;

typedef qfcl::tmp::concatenate<_QuantLib_prefix, mt19937_name>::type QuantLib_mt19937_name;

typedef named_adapter<
	standards_adapter<
			QuantLib::MersenneTwisterUniformRng
		,	unsigned long
		,	unsigned long[624]
	>, detail::QuantLib_mt19937_name> ql_mt19937;
}	// namespace detail
//! \endcond

/*! \ingroup random
	@{
*/

// named QFCL versions of QuantLib engines

class QuantLib_mt19937 : public detail::ql_mt19937
{
public:
	result_type operator()()
	{
		return e.nextInt32();
	}
};

//! @}

}	// namespace random

}	// namespace qfcl

#endif QFCL_RANDOM_QUANTLIB_MT19937_HPP

#endif USE_QUANTLIB
