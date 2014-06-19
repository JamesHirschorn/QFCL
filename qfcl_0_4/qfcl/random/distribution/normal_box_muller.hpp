/* qfcl/random/distribution/normal_box_muller.hpp
 *
 * Copyright (c) 2012 M.A. (Thijs) van den Berg, http://sitmo.com/
 * Copyright (C) 2014 James Hirschorn <James.Hirschorn@gmail.com>
 *
 * Use, modification and distribution are subject to 
 * the BOOST Software License, Version 1.0.
 * (See accompanying file LICENSE.txt)
 */

#ifndef QFCL_RANDOM_DISTRIBUTION_NORMAL_BOX_MULLER_HPP
#define QFCL_RANDOM_DISTRIBUTION_NORMAL_BOX_MULLER_HPP

/*! \file qfcl/random/distribution/normal_box_muller.hpp
	\brief Univariate normal PRNG, using the Box-Muller method.

	\author M.A. (Thijs) van den Berg, James Hirschorn
	\date February 11, 2014
*/

#include <cmath>

#include <qfcl/random/distribution/distributions.hpp>
#include <qfcl/random/distribution/qfcl_distribution_adaptor.hpp>
#include <qfcl/random/distribution/uniform_0in_1ex.hpp>
#include <qfcl/utility/named_adaptor.hpp>
#include <qfcl/utility/names.hpp>

namespace qfcl {
namespace random {
//! Version conforming to C++ standards
namespace standard {

/*! \note Would be better to use a placeholder, or else perhaps a static assert.
 */
template<typename RealType = double, typename U01_Dist = uniform_0in_1ex<RealType>>
struct normal_box_muller 
{ 
	typedef RealType result_type; 

    //! constructor
    normal_box_muller() : _valid(false)
    {
    }

	template<typename Engine>
	result_type operator()(Engine & e)
	{
        if (_valid == false) {
            _u1 = _uniform_distribution(e);
            _u2 = _uniform_distribution(e);
            _rho = std::sqrt( -2.0 * std::log( 1.0 - _u2 ));
            _valid = true;
            return _rho * std::cos(2 * 3.14159265358979323846 * _u1);
        }
        else {
            _valid = false;
            return _rho * std::sin(2 * 3.14159265358979323846 * _u1);
        }
	}
private:
    typedef U01_Dist uniform_distribution_type;
    
    uniform_distribution_type	_uniform_distribution;
    bool						_valid;

    result_type					_u1;
    result_type					_u2;
    result_type					_rho;
};

}	// namespace standard

template<typename RealType = double, typename U01_Dist = uniform_0in_1ex<RealType>>
class normal_box_muller 
	: public named_adaptor<
			qfcl_distribution_adaptor<standard::normal_box_muller<RealType, U01_Dist>
		  , variate_method<BOX_MULLER_BASIC>
		  >
		, typename qfcl::tmp::concatenate<
			string::normal_box_muller_name 
		  , typename names::template_typename<RealType>::type
		  , typename names::template_typename<U01_Dist>::type
		  >::type
		>					
{
};

}}	// namespace qfcl::random
#endif	!QFCL_RANDOM_DISTRIBUTION_NORMAL_BOX_MULLER_HPP
