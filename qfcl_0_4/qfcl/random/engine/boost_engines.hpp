/* qfcl/random/engine/boost_engines.hpp
 *
 * Copyright (C) 2014 James Hirschorn <James.Hirschorn@gmail.com>
 *
 * Use, modification and distribution are subject to 
 * the BOOST Software License, Version 1.0.
 * (See accompanying file LICENSE.txt)
 */

#ifndef QFCL_RANDOM_BOOST_ENGINES_HPP
#define QFCL_RANDOM_BOOST_ENGINES_HPP

/*! \file qfcl/random/engine/boost_engines.hpp
	\brief Wrap boost RNG engines for use with QFCL.

	\author James Hirschorn
	\date February 5, 2014
*/

#include <boost/mpl/string.hpp>

namespace qfcl {

namespace random {
	
/* boost engine names */

//! \cond
namespace detail {

//! alias
namespace mpl = boost::mpl;

typedef mpl::string<'b', 'o', 'o', 's', 't', '-'>::type _boost_prefix;
}	// namespace detail
//! \endcond


}	// namespace random

}	// namespace qfcl

#endif QFCL_RANDOM_BOOST_ENGINES_HPP
