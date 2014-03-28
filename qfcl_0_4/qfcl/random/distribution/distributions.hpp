/* qfcl/random/distribution/distributions.hpp
 *
 * Copyright (C) 2012 James Hirschorn <James.Hirschorn@gmail.com>
 *
 * Use, modification and distribution are subject to 
 * the BOOST Software License, Version 1.0.
 * (See accompanying file LICENSE.txt)
 */

#ifndef QFCL_RANDOM_DISTRIBUTION_DISTRIBUTIONS_HPP
#define QFCL_RANDOM_DISTRIBUTION_DISTRIBUTIONS_HPP

/*! \file qfcl/random/distribution/distributions.hpp
	\brief General defines for distributions.

	\author James Hirschorn
	\date February 11, 2014
*/

#include <stdexcept>
#include <string>

namespace qfcl {
namespace random {

enum variate_method_enum {
	QUANTILE,
	BOX_MULLER_BASIC,
	BOX_MULLER_POLAR
};

struct variate_method
{
	variate_method(variate_method_enum method)
		: _method(method)
	{}

	const ::std::string name() const
	{
		switch (_method)
		{
		case QUANTILE:
			return "Quantile";
		case BOX_MULLER_BASIC:
			return "Box-Muller (basic)";
		case BOX_MULLER_POLAR:
			return "Box-Muller (polar)";
		default:
			throw ::std::logic_error("bad program");
		}
	}
private:
	variate_method_enum _method;
};

}}	// namespace qfcl::random::normal

#endif !QFCL_RANDOM_DISTRIBUTION_DISTRIBUTIONS_HPP
