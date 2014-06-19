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

#include <boost/mpl/int.hpp>
#include <boost/mpl/less_equal.hpp>
#include <boost/mpl/string.hpp>
#include <boost/utility/enable_if.hpp>

#include <qfcl/utility/tmp.hpp>

namespace qfcl {
namespace string {

namespace mpl = boost::mpl;

//! the empty string
typedef mpl::string<>::type empty;

//! :: operator
typedef mpl::string<':', ':'>::type field_operator;

//! create a prefix from a string
template<typename Prefix, char prefix_char = '-'>
struct prefix
	: qfcl::tmp::concatenate<Prefix, mpl::string<prefix_char>> 
{};

//! surround by brackets, or other pair of symbols
template<typename String, char left = '(', char right = ')'>
struct brackets
	: qfcl::tmp::concatenate<mpl::string<left>, String, mpl::string<right>>
{};

//! numbers
namespace detail {

using namespace boost;
using namespace boost::mpl;

template<int n, typename Enable = void>
struct number
{};

template<int n>
struct number<n, typename enable_if<and_<
										less_equal<int_<0>, typename int_<n>::type>,
										less_equal<typename int_<n>::type, int_<9>>>>::type>
	: mpl::string<'0' + n>
{};
}	// namespace detail

template<int n>
struct number : public detail::number<n>
{};

// QuantLib
typedef boost::mpl::string<'Q', 'u', 'a', 'n', 't'>::type _Quant_string;
typedef boost::mpl::string<'L', 'i', 'b'>::type _Lib_string;
typedef qfcl::tmp::concatenate<_Quant_string, _Lib_string>::type QuantLib_string;
typedef prefix<QuantLib_string>::type QuantLib_prefix;

// Reverse
typedef boost::mpl::string<'R', 'e', 'v', 'e', 'r', 's', 'e'>::type Reverse_string;
typedef prefix<Reverse_string>::type Reverse_prefix;

// Twister
typedef boost::mpl::string<'T', 'w', 'i', 's', 't', 'e', 'r'>::type Twister_string;

// Uniform
typedef boost::mpl::string<'U', 'n', 'i', 'f', 'o', 'r', 'm'>::type Uniform_string;

// basic
typedef boost::mpl::string<'b', 'a', 's', 'i', 'c'>::type basic_string;

// boost
typedef boost::mpl::string<'b', 'o', 'o', 's', 't'>::type boost_string;
typedef prefix<boost_string>::type boost_prefix;

// continuous
typedef boost::mpl::string<'c', 'o', 'n', 't', 'i', 'n', 'u', 'o', 'u', 's'>::type continuous_string;

// double
typedef boost::mpl::string<'d', 'o', 'u', 'b', 'l', 'e'>::type double_string;

// ex
typedef boost::mpl::string<'e', 'x'>::type ex_string;

// generator
typedef boost::mpl::string<'g', 'e', 'n', 'e', 'r', 'a', 't', 'o', 'r'>::type generator_string;

// in
typedef boost::mpl::string<'i', 'n'>::type in_string;

// normal
typedef boost::mpl::string<'n', 'o', 'r', 'm', 'a', 'l'>::type normal_string;

// polar
typedef boost::mpl::string<'p', 'o', 'l', 'a', 'r'>::type polar_string;

// quantile
typedef boost::mpl::string<'q', 'u', 'a', 'n', 't', 'i', 'l', 'e'>::type quantile_string;

// std
typedef boost::mpl::string<'s', 't', 'd'>::type std_string;
typedef prefix<std_string>::type std_prefix;

// uniform
typedef boost::mpl::string<'u', 'n', 'i', 'f', 'o', 'r', 'm'>::type uniform_string;

// variate
typedef boost::mpl::string<'v', 'a', 'r', 'i', 'a', 't', 'e'>::type variate_string;

}}	// namespace qfcl::string

#endif !QFCL_MISCELLANEOUS_STRINGS_HPP


