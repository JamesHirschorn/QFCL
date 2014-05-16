/* qfcl/random/distribution/distributions.hpp
 *
 * Copyright (C) 2012 James Hirschorn <James.Hirschorn@gmail.com>
 *
 * Use, modification and distribution are subject to 
 * the BOOST Software License, Version 1.0.
 * (See accompanying file LICENSE.txt)
 */

#ifndef QFCL_RANDOM_DISTRIBUTION_DISTRIBUTIONS_HPP
#define QFCL_RANDOM_DISTRIBUTION_DISTRIBUTIONS_HPP

/*! \file qfcl/random/distribution/distributions.hpp
	\brief General defines for distributions.

	\author James Hirschorn
	\date February 11, 2014
*/

#include <stdexcept>
#include <string>

#include <boost/mpl/string.hpp>

#include <qfcl/miscellaneous/strings.hpp>
#include <qfcl/utility/tmp.hpp>

namespace qfcl {

namespace string {

using namespace qfcl::tmp;

typedef concatenate<prefix<normal_string, '_'>::type, quantile_string>::type normal_quantile_name;
typedef boost::mpl::string<'B', 'o', 'x'>::type Box_string;
typedef boost::mpl::string<'M', 'u', 'l', 'l', 'e', 'r'>::type Muller_string;
typedef concatenate<prefix<normal_string, '_'>::type, prefix<Box_string>::type, Muller_string>::type 
	normal_box_muller_name;
typedef concatenate<prefix<normal_box_muller_name, '_'>::type, polar_string>::type
	normal_box_muller_polar_name;
typedef concatenate<prefix<normal_box_muller_name, ' '>::type, brackets<basic_string>::type> box_muller_basic_method_name;

}	// namespace string

namespace random {

enum variate_method_enum {
	QUANTILE,
	BOX_MULLER_BASIC,
	BOX_MULLER_POLAR
};

template<variate_method_enum method>
struct variate_method
{
};

template<>
struct variate_method<QUANTILE>
{
	typedef string::quantile_string	name;
};

template<>
struct variate_method<BOX_MULLER_BASIC>
{
	typedef string::box_muller_basic_method_name name;
};

template<>
struct variate_method<BOX_MULLER_POLAR>
{
	typedef string::normal_box_muller_polar_name name;
};

}	// namespace random

}	// namespace qfcl

#endif !QFCL_RANDOM_DISTRIBUTION_DISTRIBUTIONS_HPP
