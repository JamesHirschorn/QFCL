/* qfcl/random/engine/detail/strings.hpp
 *
 * Copyright (C) 2014 James Hirschorn <James.Hirschorn@gmail.com>
 *
 * Use, modification and distribution are subject to 
 * the BOOST Software License, Version 1.0.
 * (See accompanying file LICENSE.txt)
 */

#ifndef QFCL_RANDOM_ENGINE_STRINGS
#define QFCL_RANDOM_ENGINE_STRINGS

/*! \file qfcl/random/engine/detail/strings.hpp
	\brief meta-strings for engines, e.g. engine names

	\author James Hirschorn
	\date June 9, 2014
*/

#include <qfcl/utility/tmp.hpp>

namespace qfcl {
namespace string {

//! alias
namespace mpl = boost::mpl;

// abuse of language until there is support multicharacter literals of arbitrary length
typedef mpl::string<'M', 'T'>::type _mt_prefix;
typedef mpl::string<'1', '1', '2', '1', '3'>::type _11213_string;
typedef mpl::string<'1', '9', '9', '3', '7'>::type _19937_string;
typedef mpl::string<'-', '6', '4'>::type _64_suffix;

typedef mpl::string<'M', 'e', 'r', 's', 'e', 'n', 'n', 'e'>::type Mersenne_string;

typedef qfcl::tmp::concatenate< _mt_prefix, _11213_string, mpl::string<'A'>::type >::type mt11213a_name;
typedef qfcl::tmp::concatenate< _mt_prefix, _11213_string, mpl::string<'B'>::type >::type mt11213b_name;
typedef qfcl::tmp::concatenate< _mt_prefix, _19937_string >::type mt19937_name;
typedef qfcl::tmp::concatenate< mt19937_name, _64_suffix >::type mt19937_64_name;
}}	// namespace qfcl::string

#endif !QFCL_RANDOM_ENGINE_STRINGS