/* qfcl/random/distribution/boost_normal_ziggurat.hpp
 *
 * Copyright (C) 2014 James Hirschorn <James.Hirschorn@gmail.com>
 *
 * Use, modification and distribution are subject to 
 * the BOOST Software License, Version 1.0.
 * (See accompanying file LICENSE.txt)
 */

#ifndef QFCL_RANDOM_BOOST_NORMAL_ZIGGURAT_HPP
#define QFCL_RANDOM_BOOST_NORMAL_ZIGGURAT_HPP

#include <qfcl/defines.hpp>

/*! \file qfcl/random/distribution/boost_normal_ziggurat.hpp
	\brief Wraps the boost::normal_distribution distribution for use with QFCL.

	Makes it additionally satisfy the Named Concept, and provide information
	on what method is used.

	\author James Hirschorn
	\date May 19, 2014
*/

#include <boost/mpl/string.hpp>
#include <boost/random/normal_distribution.hpp>

#include <qfcl/miscellaneous/strings.hpp>
#include <qfcl/random/distribution/boost-trunk/normal_distribution.hpp>
#include <qfcl/random/distribution/distributions.hpp>
#include <qfcl/random/distribution/qfcl_distribution_adaptor.hpp>
#include <qfcl/utility/tmp.hpp>
#include <qfcl/utility/named_adaptor.hpp>
#include <qfcl/utility/names.hpp>

namespace qfcl {
namespace random {

namespace standard {
template<typename Real = double>
class boost_normal_ziggurat 
	: public boost_trunk::normal_distribution<Real>
{
};
}	// namespace standard

template<typename Real = double>
class boost_normal_ziggurat 
	: public named_adaptor<
		  qfcl_distribution_adaptor<
			  standard::boost_normal_distribution<Real>
			, variate_method<ZIGGURAT>
			>
	    , tmp::concatenate<
			  string::boost_prefix 
			, string::normal_ziggurat_name
			,  typename qfcl::names::template_typename<Real>::type
			>
		>  
{
};

namespace standard {
template<typename Engine, typename RealType>
class variate_generator<Engine, boost_normal_ziggurat<RealType>>
	: public variate_generator_base<Engine, boost_normal_ziggurat<RealType>>
{
public:
	//! ctor
	variate_generator(engine_type const& e, distribution_type const& d)
		: _e(e), _d(d)
	{}

	result_type operator()()
	{
		return _d(_e);
	}
private:
	engine_type			_e;
	distribution_type	_d;
};
}	// namespace standard

template<typename Engine, typename RealType>
class variate_generator<Engine, boost_normal_ziggurat<RealType>>
	: public qfcl_variate_generator_adaptor<
			standard::variate_generator<Engine, typename boost_normal_ziggurat<RealType>::standard_type>
		,	Engine
		,	boost_normal_ziggurat<RealType>
		>
{
	typedef qfcl_variate_generator_adaptor<
			standard::variate_generator<Engine, typename boost_normal_ziggurat<RealType>::standard_type>
		,	Engine
		,	boost_normal_ziggurat<RealType>
		> base_type;
public:
	variate_generator(
		engine_type const& e = engine_type(), 
		distribution_type const& d = distribution_type())
		: base_type(e, d)
	{}
};

}}	// namespace qfcl::random

#endif !QFCL_RANDOM_BOOST_NORMAL_ZIGGURAT_HPP