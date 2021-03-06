//  Copyright (c) 2012 M.A. (Thijs) van den Berg, http://sitmo.com/
//
//  Use, modification and distribution are subject to the BOOST Software License. 
// (See accompanying file LICENSE.txt)

#ifndef QFCL_RANDOM_DISTRIBUTION_NORMAL_BOX_MULLER_HPP
#define QFCL_RANDOM_DISTRIBUTION_NORMAL_BOX_MULLER_HPP

#include <qfcl/random/distribution/uniform_0ex_1in.hpp>
#include <qfcl/random/variate_generator.hpp>
#include <cmath>

namespace qfcl {
namespace random {

template<class RealType = double>
struct normal_box_muller { typedef RealType result_type; };

template<class Engine, class RealType >
class variate_generator<Engine, normal_box_muller<RealType> >
{
public:
    typedef Engine                      engine_type;
    typedef normal_box_muller<RealType>   distribution_type;
    typedef RealType                    result_type;
    

public:
    // constructor
    variate_generator(engine_type e, distribution_type d)
    : _eng(e), _dist(d), _valid(false), _uniform_rng(e,_uniform_distribution)
    {
    }
    
    result_type operator()() 
    {
        if (_valid == false) {
            _u1 = _uniform_rng();
            _u2 = _uniform_rng();
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
    typedef uniform_0in_1in<RealType> uniform_distribution_type;
    typedef variate_generator< engine_type, uniform_distribution_type > uniform_rng_type;

private:
    engine_type         _eng;
    distribution_type   _dist;
    
    uniform_distribution_type _uniform_distribution;
    uniform_rng_type     _uniform_rng;
    bool                _valid;

    result_type         _u1;
    result_type         _u2;
    result_type         _rho;
};

}} // namespaces
#endif
