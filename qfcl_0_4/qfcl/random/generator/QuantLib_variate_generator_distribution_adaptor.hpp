/* qfcl/random/distribution/QuantLib_variate_generator_distribution_adaptor.hpp
 *
 * Copyright (C) 2014 James Hirschorn <James.Hirschorn@gmail.com>
 *
 * Use, modification and distribution are subject to 
 * the BOOST Software License, Version 1.0.
 * (See accompanying file LICENSE.txt)
 */

#ifdef USE_QUANTLIB

#ifndef QFCL_RANDOM_DISTRIBUTION_QUANTLIB_VARIATE_GENERATOR_DISTRIBUTION_ADAPTOR_HPP
#define QFCL_RANDOM_DISTRIBUTION_QUANTLIB_VARIATE_GENERATOR_DISTRIBUTION_ADAPTOR_HPP

/*! \file qfcl/random/distribution/QuantLib_variate_generator_distribution_adaptor.hpp
	\brief Standard distribution to QuantLib variate generator adaptor. 

	Converts a standard distribution into a variate generator usable with QuantLib.

	\author James Hirschorn
	\date March 19, 2014
*/

#include <functional>
#include <limits>

//#include <ql/math/randomnumbers/boxmullergaussianrng.hpp>

#include <ql/methods/montecarlo/sample.hpp>

#include <qfcl/random/distribution/distributions.hpp>
#include <qfcl/random/distribution/qfcl_distribution_adaptor.hpp>
//#include <qfcl/random/distribution/uniform_0in_1in.hpp>
//#include <qfcl/random/generator/QuantLib/boxmullergaussianrng.hpp>

namespace qfcl {
namespace random {
//! Version conforming to C++ standards
//namespace standard {

namespace detail {

template<typename Distribution>
class QuantLib_variate_generator_distribution_adaptor_base
{
protected:
	typedef typename Distribution::result_type result_type;
public:
	typedef ::QuantLib::Sample<result_type> sample_type;
protected:
	Distribution _dist;
};

}	// detail

/*! \brief Converts a standard distribution into a variate generator usable with QuantLib.
	
	N is the maximum number of variates that need to be stored. The adapter must implement
	this as a FIFO, which overwrites starting with last in when full.
*/
template<typename Distribution, unsigned long N = 1>
class QuantLib_variate_generator_distribution_adaptor
{
};

/*! 
	\class QuantLib_variate_generator_distribution_adaptor
	\brief Specialization for N = 0.

	N = 0 is a special case, where variates are generated as needed instead of stored. While this 
	is the most natural interface, and the only one that is truly a "variate generator", it comes
	at a performance cost.
*/
template<typename Distribution>
class QuantLib_variate_generator_distribution_adaptor<Distribution, 0>
	: public detail::QuantLib_variate_generator_distribution_adaptor_base<Distribution>
{
	typedef detail::QuantLib_variate_generator_distribution_adaptor_base<Distribution> base_type;
public:
	//! <param>e</param> will be used for further calls to <c>_variate_generator</c>.
	template<typename Engine>
	void operator()(Engine & e)
	{
		_variate_generator = std::bind(_dist, std::ref(e));
	}

	sample_type next() const
	{
		return sample_type(_variate_generator(), 1.0);
	}
private:
	std::function<result_type()> _variate_generator;
};

//! Specialization for N = 1.
template<typename Distribution>
class QuantLib_variate_generator_distribution_adaptor<Distribution, 1>
	: public detail::QuantLib_variate_generator_distribution_adaptor_base<Distribution>
{
public:
	void operator=(QuantLib_variate_generator_distribution_adaptor<Distribution, 1> const & other)
	{
		_dist = other._dist;
		_next = other._next;
	}

	typename sample_type next() const
	{
		return sample_type(_next, 1.0);
	}

	template<typename Engine>
	void operator()(Engine & e) 	
	{
		_next = _dist(e);
	}
private:
	result_type _next;
};

/*! 
	\class QuantLib_variate_generator_distribution_adaptor
	\brief Specialization for N = 2.

	This is an optimized version that uses the stack instead of the heap for speed efficiency.
*/
template<typename Distribution>
class QuantLib_variate_generator_distribution_adaptor<Distribution, 2>
	: public detail::QuantLib_variate_generator_distribution_adaptor_base<Distribution>
{
public:
	QuantLib_variate_generator_distribution_adaptor()
		: _front(0), _n(0)
	{
	}

	void operator=(QuantLib_variate_generator_distribution_adaptor<Distribution, 2> const & other)
	{
		_dist = other._dist;
		_stack0 = other._stack0;
		_stack1 = other._stack1;
		_front = other._front;
		_n = other._n;
	}

	typename sample_type next() const
	{
		if (_n > 0)	// buffer has 1 or 2 elements		
		{
			--_n;
			// wrap around
			if (--_front < 0)
				_front = 1;
			// return _stackN accordingly.
			return sample_type(_front == 0 ? _stack1 : _stack0, 1.0);
		}

		// buffer has no elements
		return sample_type(std::numeric_limits<result_type>::quiet_NaN(), 1.0);
	}

	int available_numbers() const
	{
		return _n;
	}

	template<typename Engine>
	void operator()(Engine & e) 	
	{
		if (++_n > 2)
			_n = 2;

		if (_front ^ (_n >> 1))
			_stack1 = _dist(e);
		else _stack0 = _dist(e);
	}
private:
	result_type _stack0;
	result_type _stack1;
	mutable int _front;
	mutable int _n;
};

//}	namespace standard
}}	// namespace qfcl::random
#endif	!QFCL_RANDOM_DISTRIBUTION_QUANTLIB_VARIATE_GENERATOR_DISTRIBUTION_ADAPTOR_HPP

#endif	USE_QUANTLIB
