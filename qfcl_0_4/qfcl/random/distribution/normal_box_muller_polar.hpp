/* qfcl/random/distribution/normal_box_muller_polar.hpp
 *
 * Copyright (c) 2012 M.A. (Thijs) van den Berg, http://sitmo.com/
 * Copyright (C) 2014 James Hirschorn <James.Hirschorn@gmail.com>
 *
 * Use, modification and distribution are subject to 
 * the BOOST Software License, Version 1.0.
 * (See accompanying file LICENSE.txt)
 */

#ifndef QFCL_RANDOM_DISTRIBUTION_NORMAL_BOX_MULLER_POLAR_HPP
#define QFCL_RANDOM_DISTRIBUTION_NORMAL_BOX_MULLER_POLAR_HPP

/*! \file qfcl/random/distribution/normal_box_muller_polar.hpp
	\brief Univariate normal PRNG, using the polar version of Box-Muller method.

	\author M.A. (Thijs) van den Berg, James Hirschorn
	\date March 28, 2014
*/
#include <cmath>

#include <qfcl/random/distribution/distributions.hpp>
#include <qfcl/random/distribution/qfcl_distribution_adaptor.hpp>
#include <qfcl/random/distribution/uniform_0in_1in.hpp>
#include <qfcl/random/generator/qfcl_variate_generator_adaptor.hpp>
#include <qfcl/utility/named_adaptor.hpp>
#include <qfcl/utility/names.hpp>
#include <qfcl/utility/standard.hpp>

namespace qfcl {
namespace random {
//! Version conforming to C++ standards
namespace standard {

template<typename RealType = double, typename U01_Dist = uniform_0in_1in<RealType>>
struct normal_box_muller_polar 
{ 
	typedef RealType result_type; 

    //! constructor
    normal_box_muller_polar() : _valid(false)
    {
    }

	template<typename Engine>
	result_type operator()(Engine & e)
	{
        if (_valid == false) {
            do {
                _u1 = 2.0 * _uniform_distribution(e) - 1.0;
                _u2 = 2.0 * _uniform_distribution(e) - 1.0;
                _r = _u1*_u1 + _u2*_u2;
                _valid = ( (_r>0.0) && (_r<1.0) );
            } while (_valid == false);

            _factor = std::sqrt((-2.0 * std::log(_r)) / _r);
            _cached_value = _factor * _u2;
            return _factor * _u1;
        }
        _valid = false;
        return _cached_value;	
	}
private:
    typedef U01_Dist uniform_distribution_type;
    
    uniform_distribution_type	_uniform_distribution;
    bool						_valid;

    result_type					_u1;
    result_type					_u2;
    result_type					_r;
    result_type					_factor;
    result_type					_cached_value;
};

}	// namespace standard

template<typename RealType = double, typename U01_Dist = uniform_0in_1in<RealType>>
class normal_box_muller_polar 
	: public named_adaptor<
			qfcl_distribution_adaptor<
				standard::normal_box_muller_polar<RealType, U01_Dist>
			,	variate_method<BOX_MULLER_POLAR>
			>
		,   typename qfcl::tmp::concatenate<
				string::normal_box_muller_polar_name 
			,	typename qfcl::names::template_typename<RealType>::type
			,	typename names::template_typename<U01_Dist>::type
			>::type
		>
{
};

/** variate generator */

namespace standard {
template<typename Engine, typename RealType, typename U01_Dist>
class variate_generator<Engine, normal_box_muller_polar<RealType, U01_Dist>>
	: public variate_generator_base<Engine, normal_box_muller_polar<RealType, U01_Dist>>
{
public:
	//! ctor
	variate_generator(engine_type const& e, distribution_type const& d)
		: _uniform_generator(e), _valid(false)
		//  _e(e), _d(d)
	{}

	result_type operator()()
	{
        if (_valid == false) {
            do {
                _u1 = 2.0 * _uniform_generator() - 1.0;
                _u2 = 2.0 * _uniform_generator() - 1.0;
                _r = _u1*_u1 + _u2*_u2;
                _valid = ( (_r>0.0) && (_r<1.0) );
            } while (_valid == false);

            _factor = std::sqrt((-2.0 * std::log(_r)) / _r);
            _cached_value = _factor * _u2;
            return _factor * _u1;
        }
        _valid = false;
        return _cached_value;	
	}
private:
	// We need the version from the standard namespace.
    typedef typename standard_type<U01_Dist>::type uniform_distribution_type;
    typedef variate_generator<engine_type, uniform_distribution_type> uniform_variate_generator_type;

	uniform_variate_generator_type	_uniform_generator;
 	//engine_type					_e;

    bool						_valid;
    result_type					_u1;
    result_type					_u2;
    result_type					_r;
    result_type					_factor;
    result_type					_cached_value;
};
}	// namespace standard

template<typename Engine, typename RealType, typename U01_Dist>
class variate_generator<Engine, normal_box_muller_polar<RealType, U01_Dist>>
	: public qfcl_variate_generator_adaptor<
			standard::variate_generator<Engine, typename normal_box_muller_polar<RealType, U01_Dist>::standard_type>
		,	Engine
		,	normal_box_muller_polar<RealType, U01_Dist>
		>
{
	typedef qfcl_variate_generator_adaptor<
			standard::variate_generator<Engine, typename normal_box_muller_polar<RealType, U01_Dist>::standard_type>
		,	Engine
		,	normal_box_muller_polar<RealType, U01_Dist>
		> base_type;
public:
	variate_generator(
		Engine const& e = Engine(), 
		normal_box_muller_polar<RealType, U01_Dist> const& d = normal_box_muller_polar<RealType, U01_Dist>())
		: base_type(e, d)
	{}
};

}} // namespace::qfcl::random
#endif	!QFCL_RANDOM_DISTRIBUTION_NORMAL_BOX_MULLER_POLAR_HPP
