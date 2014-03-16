/* qfcl/random/distribution/normal_box_muller.hpp
 *
 * Copyright (c) 2012 M.A. (Thijs) van den Berg, http://sitmo.com/
 * Copyright (C) 2012 James Hirschorn <James.Hirschorn@gmail.com>
 *
 * Use, modification and distribution are subject to 
 * the BOOST Software License, Version 1.0.
 * (See accompanying file LICENSE.txt)
 */

#ifndef QFCL_RANDOM_DISTRIBUTION_NORMAL_BOX_MULLER_HPP
#define QFCL_RANDOM_DISTRIBUTION_NORMAL_BOX_MULLER_HPP

/*! \file qfcl/random/distribution/normal_box_muller.hpp
	\brief Univariate normal PRNG, using the Box-Muller method.

	\author James Hirschorn
	\date February 11, 2014
*/

#include <qfcl/random/distribution/qfcl_distribution_adaptor.hpp>
#include <qfcl/random/distribution/uniform_0in_1in.hpp>
//#include <qfcl/random/variate_generator.hpp>
#include <cmath>

#include "distributions.hpp"

namespace qfcl {
namespace random {
//! Version conforming to C++ standards
namespace standard {

template<class RealType = double>
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
            _rho = ::std::sqrt( -2.0 * ::std::log( 1.0 - _u2 ));
            _valid = true;
            return _rho * ::std::cos(2 * 3.14159265358979323846 * _u1);
        }
        else {
            _valid = false;
            return _rho * ::std::sin(2 * 3.14159265358979323846 * _u1);
        }
	}
private:
    typedef uniform_0in_1in<RealType> uniform_distribution_type;
    
    uniform_distribution_type	_uniform_distribution;
    bool						_valid;

    result_type					_u1;
    result_type					_u2;
    result_type					_rho;
};

}	// namespace standard

//struct test_class
//{};
template<class RealType = double>
class normal_box_muller 
	: public qfcl_distribution_adaptor<standard::normal_box_muller<RealType>>
{
	typedef qfcl_distribution_adaptor<standard::normal_box_muller<RealType>>
		base_type;
public:
	static const variate_method method; 
};
//template<typename RealType = double>
//struct normal_box_muller : qfcl_distribution_adaptor<test_class> //standard::normal_box_muller<RealType>>
//{
//};

template<typename RealType>
//const variate_method qfcl_distribution_adaptor<RealType, double>::method = BOX_MULLER;
//const variate_method qfcl_distribution_adaptor<standard::normal_box_muller<RealType>, standard::normal_box_muller<RealType>::result_type>::method = BOX_MULLER;
//const variate_method normal_box_muller<RealType>::base_type::method = BOX_MULLER;
const variate_method normal_box_muller<RealType>::method = BOX_MULLER;

}}	// namespace qfcl::random
#endif	!QFCL_RANDOM_DISTRIBUTION_NORMAL_BOX_MULLER_HPP
