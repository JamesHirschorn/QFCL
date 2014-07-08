/* qfcl/random/engine/qfcl_engine_adapter.hpp
 *
 * Copyright (C) 2012 James Hirschorn <James.Hirschorn@gmail.com>
 *
 * Use, modification and distribution are subject to 
 * the BOOST Software License, Version 1.0.
 * (See accompanying file LICENSE.txt)
 */

#ifndef QFCL_RANDOM_QFCL_ENGINE_ADAPTER_HPP
#define QFCL_RANDOM_QFCL_ENGINE_ADAPTER_HPP

/*! \file qfcl/random/engine/qfcl_engine_adapter.hpp
	\brief Adapts a foreign generator into one satisfying QFCL standards.

	\author James Hirschorn
	\date February 5, 2014
*/

#include <utility>

namespace qfcl {

namespace random {

template<typename Engine, typename ResultType, typename StateType>
class qfcl_engine_adapter //: public Engine::EngineTraits
{
public:
	// use perfect forwarding for constructors
	qfcl_engine_adapter() : e() {}

	template<typename T1>
	explicit qfcl_engine_adapter(T1 && a1) 
		: e( std::forward<T1>(a1) ) {}

	template<typename T1, typename T2>
	qfcl_engine_adapter(T1 && a1, T2 && a2) 
		: e( std::forward<T1>(a1), std::forward<T2>(a2) ) {}

	typedef ResultType result_type;
	typedef StateType state;

	Engine get_engine() const
	{
		return e;
	}
protected:
	Engine e;
};

}	// namespace random

}	// namespace qfcl

#endif	QFCL_RANDOM_QFCL_ENGINE_ADAPTER_HPP
