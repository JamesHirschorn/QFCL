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

typedef boost::mpl::string<'Q', 'u', 'a', 'n', 't'>::type _Quant_string;
typedef boost::mpl::string<'L', 'i', 'b'>::type _Lib_string;
typedef qfcl::tmp::concatenate<_Quant_string, _Lib_string, boost::mpl::string<'-'>::type>::type QuantLib_string;

}	// namespace qfcl

#endif !QFCL_MISCELLANEOUS_STRINGS_HPP


