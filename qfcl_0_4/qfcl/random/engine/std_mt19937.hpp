/* qfcl/random/engine/std_mt19937.hpp
 *
 * Copyright (C) 2014 James Hirschorn <James.Hirschorn@gmail.com>
 *
 * Use, modification and distribution are subject to 
 * the BOOST Software License, Version 1.0.
 * (See accompanying file LICENSE.txt)
 */

#ifdef USE_CPP11

#ifndef QFCL_RANDOM_STD_MT19937_HPP
#define QFCL_RANDOM_STD_MT19937_HPP

/*! \file qfcl/random/engine/std_mt19937.hpp
	\brief Wraps the C++11 standard generator to obtain one compliant with QFCL.

	Makes it satisfy the Named concept.

	\author James Hirschorn
	\date February 6, 2014
*/

#include <random>

#include <boost/mpl/string.hpp>

#include <qfcl/utility/tmp.hpp>
#include <qfcl/utility/named_adapter.hpp>

#include "mersenne_twister.hpp"

namespace qfcl {

namespace random {
/* QuantLib engine names */

//! \cond
namespace detail {

//! alias
namespace mpl = boost::mpl;

typedef mpl::string<'s', 't', 'd', '-'>::type _std_prefix;

typedef qfcl::tmp::concatenate<_std_prefix, mt19937_name>::type std_mt19937_name;
}	// namespace detail
//! \endcond

/*! \ingroup random
	@{
*/

// named versions of std engines

typedef named_adapter<std::mt19937, detail::std_mt19937_name> std_mt19937;

//! @}

}	// namespace random

}	// namespace qfcl

#endif QFCL_RANDOM_STD_MT19937_HPP

#endif USE_CPP11
