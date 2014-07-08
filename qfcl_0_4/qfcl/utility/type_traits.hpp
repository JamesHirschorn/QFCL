/* qfcl/utility/type_traits.hpp
 *
 * Copyright (C) 2012 James Hirschorn <James.Hirschorn@gmail.com>
 *
 * Use, modification and distribution are subject to 
 * the BOOST Software License, Version 1.0.
 * (See accompanying file LICENSE.txt)
 */

#ifndef QFCL_TYPE_TRAITS_HPP
#define QFCL_TYPE_TRAITS_HPP

/*! \file qfcl/utility/type_traits.hpp
	\brief Additional type traits.

	\author James Hirschorn
	\date September 28, 2012
*/

#include <boost/mpl/bool.hpp>
#include <boost/type_traits.hpp>

#include <qfcl/defines.hpp>

namespace qfcl {

namespace traits {

namespace mpl = boost::mpl;

namespace detail {
	typedef char RT1; 
	typedef struct { char a[2]; } RT2;

	template<typename T, typename T::value_type>
	struct integral_constant_helper {};

	template<typename T>
	static RT1 integral_constant_test(integral_constant_helper<T, T::value> *);
	template<typename T>
	static RT2 integral_constant_test(...);
}	// namespace detail

template<typename T>
struct is_integral_constant 
    : mpl::bool_<sizeof(detail::integral_constant_test<T>(nullptr)) == 1>
{
};

#define generate_has_type_member_test(type)				\
  namespace qfcl {							\
  namespace traits {							\
  namespace detail {							\
    template<typename T>						\
    static RT1 type##_test(typename T::type const *);			\
    template<typename T>						\
    static RT2 type##_test(...);					\
  }									\
  \
  template<typename T>							\
  struct has_##type##_type : mpl::bool_<sizeof(detail::type##_test<T>(nullptr)) == 1> \
  {									\
  };									\
}} 

}}  // namespace qfcl::traits

generate_has_type_member_test(name)

namespace qfcl {
namespace traits {

  //! whether the type \c T satisfies the \c Named concept
  template<typename T>
  struct is_named : has_name_type<T>
  {};

}	// namespace traits

}	// namespace qfcl

#endif	// QFCL_TYPE_TRAITS_HPP
