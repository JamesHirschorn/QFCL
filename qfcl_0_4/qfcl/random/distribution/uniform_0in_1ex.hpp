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
#include <qfcl/random/variate_generator.hpp>
#include <qfcl/random/distribution/qfcl_distribution_adaptor.hpp>
#include <qfcl/random/distribution/distributions.hpp>
#include <qfcl/utility/tmp.hpp>
#include <qfcl/utility/named_adapter.hpp>
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
        _factor = result_type( (e.max)() - (e.min)() );
        _factor += 1;
        _factor = 1/_factor;
		return result_type( e() - (e.min)() ) * _factor;
	}
private:
    result_type         _factor;
};
}	// namespace standard


template<typename RealType = double>
class uniform_0in_1ex
	: public named_adapter<
		  qfcl_distribution_adaptor<qfcl::random::standard::uniform_0in_1ex<RealType>>
		, tmp::concatenate<
			  string::prefix<string::uniform_string, '_'>::type
			, string::prefix<tmp::concatenate<string::number<0>::type, string::in_string>::type, '_'>::type
			, string::number<1>::type
			, string::ex_string
			, typename qfcl::names::template_typename<RealType>::type>>
{
public:
	static const variate_method method; 
};

template<typename RealType>
const variate_method uniform_0in_1ex<RealType>::method = QUANTILE;

// improved efficiently
template<class Engine, class RealType >
class variate_generator<Engine, uniform_0in_1ex<RealType> >
{
public:
    typedef Engine                      engine_type;
    typedef uniform_0in_1ex<RealType>   distribution_type;
    typedef RealType                    result_type;

    // constructor
    // if, min==1, max==3 then  [ 0/3, 1/3, 2/3 ]
    variate_generator(engine_type e, distribution_type d) : _eng(e), _dist(d)
    { 
        _factor = result_type( (_eng.max)() - (_eng.min)() );
        _factor += 1;
        _factor = 1/_factor;
    }
    
    result_type operator()() 
    { return result_type( _eng() - (_eng.min)() ) * _factor; }

private:
    engine_type         _eng;
    distribution_type   _dist;
    result_type         _factor;
};

}}	// namespace qfcl::random
#endif	!QFCL_RANDOM_UNIFORM_0IN_1EX_HPP
