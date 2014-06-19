/* qfcl/random/distribution/uniform_0in_1ex.hpp
 *
 * Copyright (c) 2012 M.A. (Thijs) van den Berg, http://sitmo.com/
 * Copyright (C) 2014 James Hirschorn <James.Hirschorn@gmail.com>
 *
 * Use, modification and distribution are subject to 
 * the BOOST Software License, Version 1.0.
 * (See accompanying file LICENSE.txt)
 */

#ifndef QFCL_RANDOM_UNIFORM_0IN_1EX_HPP
#define QFCL_RANDOM_UNIFORM_0IN_1EX_HPP

/*! \file qfcl/random/distribution/uniform_0in_1ex.hpp
	\brief U([0,1]) variate distribution.

	\author M.A. (Thijs) van den Berg, James Hirschorn
	\date April 22, 2014
*/

#include <qfcl/miscellaneous/strings.hpp>
#include <qfcl/random/distribution/qfcl_distribution_adaptor.hpp>
#include <qfcl/random/distribution/distributions.hpp>
#include <qfcl/random/generator/qfcl_variate_generator_adaptor.hpp>
#include <qfcl/random/generator/variate_generator.hpp>
#include <qfcl/utility/tmp.hpp>
#include <qfcl/utility/named_adaptor.hpp>
#include <qfcl/utility/names.hpp>

namespace qfcl {
namespace random {
//! Version conforming to C++ standards
namespace standard {
template<class RealType = double>
class uniform_0in_1ex 
{ 
public:
	typedef RealType result_type; 

	template<typename Engine>
	result_type operator()(Engine & e)
	{
        _factor = result_type( Engine::max() - Engine::min() );
        _factor += 1;
        _factor = 1 / _factor;
		return ( e() - Engine::min() ) * _factor;
	}
private:
    result_type         _factor;
};
}	// namespace standard


template<typename RealType = double>
class uniform_0in_1ex
	: public named_adaptor<
		  qfcl_distribution_adaptor<
			  qfcl::random::standard::uniform_0in_1ex<RealType>
			, variate_method<QUANTILE>>
		, tmp::concatenate<
			  string::prefix<string::uniform_string, '_'>::type
			, string::prefix<tmp::concatenate<string::number<0>::type, string::in_string>::type, '_'>::type
			, string::number<1>::type
			, string::ex_string
			, typename qfcl::names::template_typename<RealType>::type>>
{
};

// improved efficiently through use of statics

namespace standard {
template<typename Engine, typename RealType>
class variate_generator<Engine, uniform_0in_1ex<RealType> >
	: public variate_generator_base<Engine, uniform_0in_1ex<RealType> >
{
public:
	//! default ctor
	variate_generator() 
	{
		initialize();
	}
    // constructors
    variate_generator(engine_type const& e, distribution_type const& d = distribution_type()) 
		: _eng(e)//, _dist(d)
    { 
		initialize();
    }
    
    result_type operator()() 
    { 
		return ( _eng() - engine_type::min() ) * _factor; 
	}

private:
    engine_type         _eng;
    //distribution_type   _dist;
    static result_type  _factor;
	static bool			_initialized;

	// if, min==1, max==3 then  [ 0/3, 1/3, 2/3 ]
	static void initialize()
	{
		if (_initialized)
			return;

		_factor = 1 / ( result_type( engine_type::max() - engine_type::min() ) + 1 );
		_initialized = true;
	}
};

template<typename Engine, typename RealType>
RealType variate_generator<Engine, uniform_0in_1ex<RealType> >::_factor;

template<typename Engine, typename RealType>
bool variate_generator<Engine, uniform_0in_1ex<RealType> >::_initialized = false;
}	// namespace standard

template<typename Engine, typename RealType>
class variate_generator<Engine, uniform_0in_1ex<RealType> >
	: public qfcl_variate_generator_adaptor<
			standard::variate_generator<Engine, standard::uniform_0in_1ex<RealType> >
		,	Engine
		,	uniform_0in_1ex<RealType>
		>
{
	typedef qfcl_variate_generator_adaptor<
			standard::variate_generator<Engine, standard::uniform_0in_1ex<RealType> >
		,	Engine
		,	uniform_0in_1ex<RealType>
		> base_type;
public:
	variate_generator(Engine const& e = Engine(), uniform_0in_1ex<RealType> const& d = uniform_0in_1ex<RealType>())
		: base_type(e, d)
	{}
};

}}	// namespace qfcl::random
#endif	!QFCL_RANDOM_UNIFORM_0IN_1EX_HPP
