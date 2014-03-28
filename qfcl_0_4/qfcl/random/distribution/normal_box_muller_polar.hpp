/* qfcl/random/distribution/normal_box_muller_polar.hpp
 *
 * Copyright (c) 2012 M.A. (Thijs) van den Berg, http://sitmo.com/
 * Copyright (C) 2012 James Hirschorn <James.Hirschorn@gmail.com>
 *
 * Use, modification and distribution are subject to 
 * the BOOST Software License, Version 1.0.
 * (See accompanying file LICENSE.txt)
 */

#ifndef QFCL_RANDOM_DISTRIBUTION_NORMAL_BOX_MULLER_POLAR_HPP
#define QFCL_RANDOM_DISTRIBUTION_NORMAL_BOX_MULLER_POLAR_HPP

/*! \file qfcl/random/distribution/normal_box_muller_polar.hpp
	\brief Univariate normal PRNG, using the polar version of Box-Muller method.

	\author James Hirschorn
	\date March 28, 2014
*/

#include <qfcl/random/distribution/uniform_0in_1in.hpp>
#include <qfcl/random/variate_generator.hpp>
#include <cmath>

namespace qfcl {
namespace random {
//! Version conforming to C++ standards
namespace standard {

template<typename RealType = double>
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
    typedef uniform_0in_1in<RealType> uniform_distribution_type;
    
    uniform_distribution_type	_uniform_distribution;
    bool						_valid;

    result_type         _u1;
    result_type         _u2;
    result_type         _r;
    result_type         _factor;
    result_type         _cached_value;
};

}	// namespace standard

template<typename RealType = double>
class normal_box_muller_polar 
	: public qfcl_distribution_adaptor<standard::normal_box_muller_polar<RealType>>
{
	typedef qfcl_distribution_adaptor<standard::normal_box_muller_polar<RealType>>
		base_type;
public:
	static const variate_method method; 
};

template<typename RealType>
const variate_method normal_box_muller_polar<RealType>::method = BOX_MULLER_POLAR;

}} // namespace::qfcl::random
#endif	!QFCL_RANDOM_DISTRIBUTION_NORMAL_BOX_MULLER_POLAR_HPP
