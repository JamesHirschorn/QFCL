/* qfcl/random/distribution/boost_normal_distribution.hpp
 *
 * Copyright (C) 2012 James Hirschorn <James.Hirschorn@gmail.com>
 *
 * Use, modification and distribution are subject to 
 * the BOOST Software License, Version 1.0.
 * (See accompanying file LICENSE.txt)
 */

#ifndef QFCL_RANDOM_BOOST_NORMAL_DISTRIBUTION_HPP
#define QFCL_RANDOM_BOOST_NORMAL_DISTRIBUTION_HPP

#include <qfcl/defines.hpp>

/*! \file qfcl/random/distribution/boost_normal_distribution.hpp
	\brief Wraps the boost::normal_distribution distribution for use with QFCL.

	Makes it additionally satisfy the Named Concept, and provide information
	on what method is used.

	\author James Hirschorn
	\date February 12, 2014
*/

#include <boost/mpl/string.hpp>
#include <boost/random/normal_distribution.hpp>

#include <qfcl/miscellaneous/strings.hpp>
#include <qfcl/random/distribution/distributions.hpp>
#include <qfcl/random/distribution/qfcl_distribution_adaptor.hpp>
#include <qfcl/utility/tmp.hpp>
#include <qfcl/utility/named_adapter.hpp>
#include <qfcl/utility/names.hpp>

namespace qfcl {

namespace random {

template<typename Real = double>
class boost_normal_distribution 
	: public named_adapter<
		qfcl_distribution_adaptor<boost::normal_distribution<Real>>,
		tmp::concatenate<
			string::boost_prefix, 
			string::normal_box_muller_name,
			typename qfcl::names::template_typename<Real>::type>>  
{
public:
	// don't know why this should be needed (compiler bug?)
	static const variate_method method;
};

template<typename Real>
const variate_method boost_normal_distribution<Real>::method = BOX_MULLER_BASIC;

}}	// namespace qfcl::random

#endif !QFCL_RANDOM_BOOST_NORMAL_DISTRIBUTION_HPP
