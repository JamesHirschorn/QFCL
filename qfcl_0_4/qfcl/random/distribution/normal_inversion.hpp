/* qfcl/random/distribution/normal_inversion.hpp
 *
 * Copyright (c) 2012 M.A. (Thijs) van den Berg, http://sitmo.com/
 * Copyright (C) 2014 James Hirschorn <James.Hirschorn@gmail.com>
 *
 * Use, modification and distribution are subject to 
 * the BOOST Software License, Version 1.0.
 * (See accompanying file LICENSE.txt)
 */

#ifndef QFCL_RANDOM_DISTRIBUTION_NORMAL_INVERSION_HPP
#define QFCL_RANDOM_DISTRIBUTION_NORMAL_INVERSION_HPP

/*! \file qfcl/random/distribution/normal_inversion.hpp
	\brief Univariate normal PRNG, using the quantile method.

	\author M.A. (Thijs) van den Berg, James Hirschorn
	\date April 22, 2014
*/

#include <cmath>

#include <qfcl/miscellaneous/strings.hpp>
#include <qfcl/random/distribution/distributions.hpp>
#include <qfcl/random/distribution/qfcl_distribution_adaptor.hpp>
#include <qfcl/random/distribution/uniform_0ex_1ex.hpp>
#include <qfcl/random/distribution/normal_quantile.hpp>
#include <qfcl/utility/named_adaptor.hpp>
#include <qfcl/utility/names.hpp>

namespace qfcl {
namespace random {

namespace detail {

    double normal_inv(double p)
    {
        const double A1 = -3.969683028665376e+01;
        const double A2 =  2.209460984245205e+02;
        const double A3 = -2.759285104469687e+02;
        const double A4 =  1.383577518672690e+02;
        const double A5 = -3.066479806614716e+01;
        const double A6 =  2.506628277459239e+00;
        
        const double B1 = -5.447609879822406e+01;
        const double B2 =  1.615858368580409e+02;
        const double B3 = -1.556989798598866e+02;
        const double B4 =  6.680131188771972e+01;
        const double B5 = -1.328068155288572e+01;
        
        const double C1 = -7.784894002430293e-03;
        const double C2 = -3.223964580411365e-01;
        const double C3 = -2.400758277161838e+00;
        const double C4 = -2.549732539343734e+00;
        const double C5 =  4.374664141464968e+00;
        const double C6 =  2.938163982698783e+00;
        
        const double D1 =  7.784695709041462e-03;
        const double D2 =  3.224671290700398e-01;
        const double D3 =  2.445134137142996e+00;
        const double D4 =  3.754408661907416e+00;
        
        const double P_LOW =    0.02425;
        const double P_HIGH =   0.97575; // P_high = 1 - p_low;
    
        double x;
        double q, r;
    
        if ((0.0 < p )  && (p < P_LOW)) {
            q = sqrt(-2*log(p));
            x = (((((C1*q+C2)*q+C3)*q+C4)*q+C5)*q+C6) / ((((D1*q+D2)*q+D3)*q+D4)*q+1);
        } else {
            if ((P_LOW <= p) && (p <= P_HIGH)){
               q = p - 0.5;
               r = q*q;
               x = (((((A1*r+A2)*r+A3)*r+A4)*r+A5)*r+A6)*q /(((((B1*r+B2)*r+B3)*r+B4)*r+B5)*r+1);
            } else {
                if ((P_HIGH < p)&&(p < 1.0)){
                    q = sqrt(-2*std::log(1-p));
                    x = -(((((C1*q+C2)*q+C3)*q+C4)*q+C5)*q+C6) / ((((D1*q+D2)*q+D3)*q+D4)*q+1);
                }
            }
        }
    
        return x;
    }

	template<typename RealType>
	struct quantile
	{
		quantile(RealType mu, RealType sigma)
			: _mu(mu), _sigma(sigma)
		{};

		RealType operator()(RealType x) const
		{
			return _mu + _sigma * normal_inv(x);
		}
	private:
		RealType _mu, _sigma;
	};
}	// namespace detail

//! Version conforming to C++ standards
namespace standard {

template<typename RealType = double, typename U01_Dist = uniform_0ex_1ex<RealType>>
class normal_inversion 
	: public standard::normal_quantile<qfcl::random::detail::quantile<RealType>, RealType, U01_Dist>
{ 
};

}	// namespace standard

template<typename RealType = double, typename U01_Dist = uniform_0ex_1ex<RealType>>
class normal_inversion
	: public named_adaptor<
			qfcl_distribution_adaptor<
			standard::normal_inversion<RealType, U01_Dist>
		  , variate_method<QUANTILE>
		  >
		, typename qfcl::tmp::concatenate<
			string::normal_quantile_name
		  , typename qfcl::names::template_typename<RealType>::type
		  , typename names::template_typename<U01_Dist>::type
		  >::type
		>					
{
};

//  Specialization for standard normal distribution, will give noticeably better performance?
//template<typename RealType = double>
//struct std_normal_inversion_distribution : normal_inversion_distribution<RealType>
//{
//};
//
// Keep these?
//typedef normal_inversion_distribution<double> normal_inversion;
//typedef std_normal_inversion_distribution<double> std_normal_inversion;

template<class Engine, class RealType >
class variate_generator<Engine, standard::normal_inversion<RealType> >
{
public:
	typedef Engine									engine_type;
	typedef normal_inversion<RealType>	distribution_type;
    typedef RealType								result_type;
    

public:
    // constructor
    variate_generator(engine_type e, distribution_type d)
    : _eng(e), _dist(d), _uniform_rng(e,_uniform_distribution)
    {
    }
    
    result_type operator()() 
    {
		return _dist.mu + _dist.sigma * detail::normal_inv( _uniform_rng() );
    }

private:
    typedef uniform_0ex_1ex<RealType> uniform_distribution_type;
    typedef variate_generator< engine_type, uniform_distribution_type > uniform_rng_type;

private:
    engine_type          _eng;
    distribution_type   _dist;
    
    uniform_distribution_type _uniform_distribution;
    uniform_rng_type     _uniform_rng;
};

// NOTE: First need specialized std_normal_inversion distribution?
// specialized version for (possibly) better performance
//template<class Engine, class RealType >
//class variate_generator<Engine, std_normal_inversion_distribution<RealType> >
//{
//public:
//	typedef Engine									engine_type;
//	typedef normal_inversion_distribution<RealType>	distribution_type;
//    typedef RealType								result_type;
//    
//
//public:
//    // constructor
//    variate_generator(engine_type e, distribution_type d)
//    : _eng(e), _dist(d), _uniform_rng(e,_uniform_distribution)
//    {
//    }
//    
//    result_type operator()() 
//    {
//		return detail::normal_inv( _uniform_rng() );
//    }
//
//private:
//    typedef uniform_0ex_1ex<RealType> uniform_distribution_type;
//    typedef variate_generator< engine_type, uniform_distribution_type > uniform_rng_type;
//
//private:
//    typename engine_type         _eng;
//    typename distribution_type   _dist;
//    
//    uniform_distribution_type _uniform_distribution;
//    uniform_rng_type     _uniform_rng;
//};

}}	// namespace qfcl::random
#endif	!QFCL_RANDOM_DISTRIBUTION_NORMAL_INVERSION_HPP
