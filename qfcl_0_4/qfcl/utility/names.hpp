/* qfcl/utility/names.hpp
 *
 * Copyright (C) 2012 James Hirschorn <James.Hirschorn@gmail.com>
 *
 * Use, modification and distribution are subject to 
 * the BOOST Software License, Version 1.0.
 * (See accompanying file LICENSE.txt)
 */

#ifndef QFCL_UTILITY_NAMES_HPP
#define QFCL_UTILITY_NAMES_HPP

/*! \file names.hpp
	\brief Some functions and metafunctions for handling name tags and typenames.

	\author James Hirschorn
	\date January 2, 2013
*/

#include <string>
#include <vector>

//#include <qfcl/utility/for_each.hpp>
#include <qfcl/miscellaneous/strings.hpp>
#include <qfcl/utility/tmp.hpp>
#include <qfcl/utility/type_traits.hpp>

namespace qfcl {
namespace names {
	
//! extracts the name for types modelling the \c Named Concept
template<typename T>
struct name_tag : T::name
{
};

/*! \brief extracts the "name" of the object as a char *

	Uses the \c Name field for types modelling the \c Named Concept, and the typename otherwise.
*/
template<typename T>
const char * name(T & t)
{
    return mpl::c_str<typename name_tag<T>::type>::value;
}

template<typename T>
const char * name_or_typename(T & t, typename boost::disable_if<traits::is_named<T>>::type * dummy = 0)
{
	return typeid(T).name();
}

template<typename T>
const char * name_or_typename(T & t, typename boost::enable_if<traits::is_named<T>>::type * dummy = 0)
{
	//return mpl::c_str<name<T>::type>::value;
	return name(t);
}

/*! \brief gives an mpl::string for the template type name.
	\note \param T must satisfy the Named Model, or else the definition must be specialized.
*/

static const char template_char_left	= '{';
static const char template_char_right	= '}';

template<typename T>
struct template_typename
	: string::brackets<typename name_tag<T>::type, template_char_left, template_char_right>
{};

template<>
struct template_typename<double> 
	: string::brackets<string::double_string, template_char_left, template_char_right>
{};

typedef std::vector<std::string> vector_of_strings;

namespace detail {

struct get_name_or_typenames_object
{
	get_name_or_typenames_object(vector_of_strings & vos) : vos_(vos) {}

	template<typename T>
	void operator()(T t) const
	{
		vos_.push_back(name_or_typename(t));
	}
private:
    vector_of_strings & vos_;
};

}	// namespace detail

template<typename List>
vector_of_strings get_name_or_typenames()
{
//	typedef std::vector<std::string> result_type;

	vector_of_strings result;

	//qfcl::tmp
		mpl::for_each<List>(detail::get_name_or_typenames_object(result));

	return result;
}

namespace detail {

//! create a \c vector_of_strings from a \c Sequence of \c mpl::string types
struct get_mpl_strings_object
{
	get_mpl_strings_object(vector_of_strings & vos) : vos_(vos) {}

	template<typename T>
	void operator()(T)
	{
		vos_.push_back( mpl::c_str<T>::value );
	}
private:
    vector_of_strings & vos_;
};

}	// namespace detail

template<typename SequenceOfStrings>
vector_of_strings get_mpl_strings()
{
	vector_of_strings result;

	qfcl::tmp::for_each<SequenceOfStrings>(detail::get_mpl_strings_object(boost::ref(result)));

	return result;
}

//! creates a vector of names from a sequence of models of the Named concept
template<typename List>
vector_of_strings get_names()
{
	// create an mpl::list of names (compile time)
    typedef typename mpl::reverse_fold< List,
							   qfcl::tmp::empty_list, 
							   mpl::push_front< mpl::_1, name_tag<mpl::_2> > 
							 >::type names;
	// create a vector_of_strings (run time)
	return get_mpl_strings<names>();
}

}	// namespace names
}	// namespace qfcl

#endif // !QFCL_UTILITY_NAMES_HPP
