//  Copyright (c) 2012 M.A. (Thijs) van den Berg, http://sitmo.com/
//
//  Use, modification and distribution are subject to the BOOST Software License. 
// (See accompanying file LICENSE.txt)

#ifndef QFCL_RANDOM_VARIATE_GENERATOR_HPP
#define QFCL_RANDOM_VARIATE_GENERATOR_HPP

// May 5, 2014

namespace qfcl {
namespace random {

template<typename Engine, typename Distribution>
class variate_generator_base
{
public:
	typedef	Engine								engine_type;
	typedef Distribution						distribution_type;
	typedef typename Distribution::result_type	result_type;
};

namespace standard {
template<typename Engine, typename Distribution>
class variate_generator
{};
}	// namespace standard

template<class Engine, class Distribution >
class variate_generator
	: public variate_generator_base<Engine, Distribution>
{
public:
    variate_generator(Engine e, Distribution d)
    {
        Engine::UNSUPPORTED_ENGINE_DISTRIBUTION_COMBINATION;
        Distribution::UNSUPPORTED_ENGINE_DISTRIBUTION_COMBINATION;
    }
};

}} // namespaces
#endif //
