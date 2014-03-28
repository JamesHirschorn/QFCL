/* qfcl/miscellaneous/strings.hpp
 *
 * Copyright (C) 2014 James Hirschorn <James.Hirschorn@gmail.com>
 *
 * Use, modification and distribution are subject to 
 * the BOOST Software License, Version 1.0.
 * (See accompanying file LICENSE.txt)
 */

#ifndef QFCL_MISCELLANEOUS_STRINGS_HPP
#define QFCL_MISCELLANEOUS_STRINGS_HPP

#include <boost/mpl/string.hpp>

namespace qfcl {
namespace string {

//! create a prefix from a string
template<typename Prefix>
struct prefix
	: qfcl::tmp::concatenate<Prefix, mpl::string<'-'>::type>
{
};
	
// boost
typedef boost::mpl::string<'b', 'o', 'o', 's', 't'>::type boost_string;
typedef prefix<boost_string>::type boost_prefix;

// QuantLib
typedef boost::mpl::string<'Q', 'u', 'a', 'n', 't'>::type _Quant_string;
typedef boost::mpl::string<'L', 'i', 'b'>::type _Lib_string;
typedef qfcl::tmp::concatenate<_Quant_string, _Lib_string>::type QuantLib_string;
typedef prefix<QuantLib_string>::type QuantLib_prefix;

// Reverse
typedef boost::mpl::string<'R', 'e', 'v', 'e', 'r', 's', 'e'>::type Reverse_string;
typedef prefix<Reverse_string>::type Reverse_prefix;

// std
typedef boost::mpl::string<'s', 't', 'd'>::type std_string;
typedef prefix<std_string>::type std_prefix;

}}	// namespace qfcl::string

#endif !QFCL_MISCELLANEOUS_STRINGS_HPP


