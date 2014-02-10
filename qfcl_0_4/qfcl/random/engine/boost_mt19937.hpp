/* qfcl/random/engine/boost_mt19937.hpp
 *
 * Copyright (C) 2012 James Hirschorn <James.Hirschorn@gmail.com>
 *
 * Use, modification and distribution are subject to 
 * the BOOST Software License, Version 1.0.
 * (See accompanying file LICENSE.txt)
 */

#ifndef QFCL_RANDOM_BOOST_MT19937_HPP
#define QFCL_RANDOM_BOOST_MT19937_HPP

/*! \file qfcl/random/engine/boost_engines.hpp
	\brief Wraps the boost::mt19937 engine for use with QFCL.

	Makes it additionally satisfy the Named Concept.

	\author James Hirschorn
	\date August 7, 2012
*/

#include <boost/mpl/string.hpp>
#include <boost/random/mersenne_twister.hpp>

#include <qfcl/utility/tmp.hpp>
#include <qfcl/utility/named_adapter.hpp>

#include "boost_engines.hpp"
#include "mersenne_twister.hpp"

namespace qfcl {

namespace random {
	
/* boost engine names */

//! \cond
namespace detail {

//! alias
namespace mpl = boost::mpl;

typedef qfcl::tmp::concatenate<_boost_prefix, mt19937_name>::type boost_mt19937_name;
}	// namespace detail
//! \endcond

/*! \ingroup random
	@{
*/

// named versions of boost engines

typedef named_adapter<boost::mt19937, detail::boost_mt19937_name> boost_mt19937;

//! @}

}	// namespace random

}	// namespace qfcl

#endif QFCL_RANDOM_BOOST_MT19937_HPP
