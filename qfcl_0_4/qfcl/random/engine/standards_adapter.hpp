/* qfcl/random/engine/standards_adapter.hpp
 *
 * Copyright (C) 2012 James Hirschorn <James.Hirschorn@gmail.com>
 *
 * Use, modification and distribution are subject to 
 * the BOOST Software License, Version 1.0.
 * (See accompanying file LICENSE.txt)
 */

#ifndef QFCL_RANDOM_STANDARDS_ADAPTER_HPP
#define QFCL_RANDOM_STANDARDS_ADAPTER_HPP

/*! \file qfcl/random/engine/standards_adapter.hpp
	\brief Adapts a foreign generator into one satisfying QFCL standards.

	\author James Hirschorn
	\date February 5, 2014
*/

#include <utility>

namespace qfcl {

namespace random {

template<typename Engine, typename result_type, typename state_type>
class standards_adapter //: public Engine::EngineTraits
{
public:
	// use perfect forwarding for constructors
	standards_adapter() : e() {}

	template<typename T1>
	explicit standards_adapter(T1 && a1) 
		: e( std::forward<T1>(a1) ) {}

	template<typename T1, typename T2>
	standards_adapter(T1 && a1, T2 && a2) 
		: e( std::forward<T1>(a1), std::forward<T2>(a2) ) {}

	typedef result_type result_type;
	typedef state_type state;
protected:
	Engine e;
};

}	// namespace random

}	// namespace qfcl

#endif	QFCL_RANDOM_STANDARDS_ADAPTER_HPP