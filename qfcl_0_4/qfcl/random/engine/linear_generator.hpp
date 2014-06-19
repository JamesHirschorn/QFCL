/* qfcl/random/engine/linear_generator.hpp
 *
 * Copyright (C) 2012 James Hirschorn <James.Hirschorn@gmail.com>
 *
 * Use, modification and distribution are subject to 
 * the BOOST Software License, Version 1.0.
 * (See accompanying file LICENSE.txt)
 */

#ifndef QFCL_RANDOM_LINEAR_GENERATOR_HPP
#define QFCL_RANDOM_LINEAR_GENERATOR_HPP

/*! \file qfcl/random/engine/linear_generator.hpp
	\brief Components for linear generators

	\author James Hirschorn
	\date March 23, 2012
*/

#include <cassert>
#include <fstream>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>

#include <boost/cstdint.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/mpl/string.hpp>
#include <boost/type_traits/is_same.hpp>
// NOTE: For debugging, to be removed
#include <boost/timer/timer.hpp>

#include <qfcl/defines.hpp>
#include <qfcl/math/bits/copy_bits.hpp>
#include <qfcl/types.hpp>
#include <qfcl/utility/tmp.hpp>
#include <qfcl/utility/io.hpp>

#include "engine.hpp"
#include "matrix.hpp"

#pragma warning(disable:4290)
//#pragma warning(disable:4519)	// default function template arguments
#pragma warning(disable:4800)

//#include "reverse_adapter.hpp"

namespace qfcl {

namespace random {

/*! \ingroup random
	@{
*/

//! tag struct for linear generators engines
struct linear_generator_engine_tag : public random_engine_tag {};

namespace detail {

template<typename charT, typename Traits, typename StateType>
std::basic_ostream<charT, Traits> & 
output(std::basic_ostream<charT, Traits> & os, const StateType & s)
{
	os << s.rep()[0];

	for (size_t i = 1; i < s.length(); ++i)
		os << ' ' <<  s.rep()[i];

	return os;
}

}	// namespace detail

//! \f$\mathbb F_m\f$-linear generator of dimension k base class
//template<typename Derived, size_t k, size_t mod>
//class linear_generator
//{
//};

// forward declarations
template<typename Derived, typename EngineTraits>
class noninvertible_linear_generator;
template<typename Derived, typename EngineTraits>
class invertible_linear_generator;

template<typename Derived, typename EngineTraits>
class linear_generator : public EngineTraits
{
	static_assert( boost::is_same<typename EngineTraits::engine_category, linear_generator_engine_tag>::value, "hello");
				  // boost::mpl::c_str<typename EngineTraits::name>::value );// << " derives from linear_generator base class, but is not a linear generator" );

public:
	typedef typename EngineTraits::result_type result_type;
    QFCL_USING_TYPE(UIntType, EngineTraits);
    using EngineTraits::default_seed;
protected:
	/** convenience definitions */
	static const size_t mod = EngineTraits::modulus;
	static const size_t w = EngineTraits::word_size;
	static const size_t n = EngineTraits::state_size;
	static const size_t r = EngineTraits::mask_bits;
 //   static const auto default_seed = EngineTraits::default_seed;
private:
	static const size_t k = n * w - r;
public:
	typedef Matrix<mod> matrix_t;
	typedef Vector<mod> vector_t;

	struct state;

	// ctor
	/*! \sa seed(UIntType)
	*/
	//linear_generator(UIntType seed_ = default_seed) {seed(seed_);}

	//! get the next random number to be generated (without changing the state)
	result_type peek(unsigned long long v) const;

	//! advance the state by \c num steps
	void discard(unsigned long long num);
	
	//! returns the transition matrix
	static matrix_t & TransitionMatrix() {return TransitionMatrix_imp();}
	//! whether the file containing the transition matrix exists
	static bool TransitionMatrix_file_exists() {return TransitionMatrix_file_exists_imp();}
	//! returns the jump matrix corresponding to the given \p jump_size
	static matrix_t & JumpMatrix(unsigned long long jump_size)
	{
		return JumpMatrix_imp(jump_size, false);
	}
	//! whether the file containing the jump matrix of jump size v exists
	static bool JumpMatrix_file_exists(unsigned long long jump_size)
	{
		return JumpMatrix_file_exists_imp(jump_size);
	}

	//! get the state of the engine
	const state getState() const {return state(x, i);}

	/*! \brief whether two mersenne twisters of the same type are \em equivalent, 
		i.e. generate an identical sequence of numbers

		It is possible for two mersenne twisters of differing type (i.e. different template parameters)
		to be equivalent. However, operator== is not defined for mersenne twisters of differing type,
		and in any case we do not know of any feasible method to check for equivalence
		in the general case.
	*/
	friend bool operator==(const linear_generator & eng1, const linear_generator & eng2)
	{
		if (eng1.i <= eng2.i)
			return eng1.equal_imp(eng2);
		else
			return eng2.equal_imp(eng1);
	}

	//! whether two mersenne twisters of the same type are not equivalent
	friend bool operator!=(const linear_generator & eng1, const linear_generator & eng2)
	{
		return !(eng1 == eng2);
	}

	//! outputs the state of the generator to a \c std::ostream
	/*! Outputs a stream of \c UIntType elements of length \c n. 
		These are the \c n numbers generated by the \c linear_generator, in order,
		preceding the current state. This uniquely determines the current state.
	*/
	template<typename charT, typename Traits>
	friend ::std::basic_ostream<charT, Traits> & 
	operator<<(::std::basic_ostream<charT, Traits> & os, 
			   const linear_generator & eng)
	{
		state s(eng.x, eng.i);

		return detail::output(os, s);
	}
	
	//! sets the generator state from a \c std::istream
	/*! The state is determined by reading \c n elements of type \c UIntType
		from the stream.
	*/
	template<typename charT, typename Traits>
	friend ::std::basic_istream<charT, Traits> & 
	operator>>(::std::basic_istream<charT, Traits> & is,
			   Derived & eng)
	{
		UIntType y[n];
		for (size_t i = 0; i < n; ++i)
			is >> y[i] >> std::ws;

		eng.seed(y);

		return is;
	}

	//! the state of a linear generator
	friend struct state;
	//! give access to selected subclasses only
	friend class noninvertible_linear_generator<Derived, EngineTraits>;
	friend class invertible_linear_generator<Derived, EngineTraits>;
private:
	//! the RNG state stored as a circular buffer
	UIntType x[n];
	//! index within the state of the next random number to be generated
	std::size_t i;

	//! the jump matrix
	static matrix_t J;
	//! whether the jump matrix has been set yet
	static bool J_set;
	//! size of the set jump matrix
	static unsigned long long computed_size;
	//! whether the set jump matrix corresponds to a reverse jump (only used for \c invertible_linear_generator s)
	static bool computed_reverse;

	//! re-seed the generator, \c seed() resets to the default seed
    void seed_imp(UIntType seed_ = EngineTraits::default_seed) {Derived::SeedInitialization(seed_, x, i);}
	//! re-seed the generator with a sequence of seeds of arbitrary length
	template<typename It>
	void seed_imp(It begin, It end) {Derived::SeedInitialization(begin, end, x, i);}
	//! common routine for seeding from a given state
	void seed_imp(const state & s);

	//! The actual implementation of <tt>operator==</tt>. Requires <tt>i <= eng.i</tt>.
	bool equal_imp(const linear_generator & eng) const;
		
	//! generate the next \c num numbers, without transforming them, after skipping ahead \c skip steps, and store the result in \c dest
	template<typename OutIt>
	static void Get(const UIntType (&x)[n], size_t i, OutIt dest, size_t num, unsigned long long skip = 0);
	//! similar to \c Get but transformed, and starts from 0 instead of 1, 
	//! i.e. begins with the number just generated; also non-static
	template<typename OutIt>
	void TransformedGet0(OutIt dest, size_t num, unsigned long long skip = 0) const;
	
	//! common routine for computing the transition matrix
	static matrix_t & TransitionMatrix_imp(bool reverse = false);
	//! common routine for checking whether the transition matrix exists
	static bool TransitionMatrix_file_exists_imp(bool reverse = false);

	//! common routine for computing the jump matrix
	static matrix_t & JumpMatrix_imp(unsigned long long v, bool reverse);
	//! common routine for checking whether the jump matrix exists
	static bool JumpMatrix_file_exists_imp(unsigned long long jump_size, bool reverse = false);

	//! the name of the file containing the transition matrix
	static std::string transition_matrix_filename(bool reverse = false);
	//! the name of the file containing the jump matrix
	static std::string jump_matrix_filename(unsigned long long jump_size, bool reverse = false);
	//! handles \c linear_generator matrix reads, computing and storing on disk when needed
	template<typename F>
    static matrix_t obtain_matrix(const std::string & filename, const F & f);

	struct transition_matrix_functor;
	struct jump_matrix_functor;
	struct reverse_jump_matrix_functor;
protected:
	//! default constructor
	// to be used as base class only
	linear_generator() {};
};

// seed
/*! Only the last \c w bits of each seed are used.
*/
template<typename Derived, typename EngineType>
inline void
linear_generator<Derived, EngineType>::seed_imp(const state & s)
{
	std::copy(&s.rep()[0], &s.rep()[n], x);
	i = 0;
}

// discard
template<typename Derived, typename EngineType>
inline void
linear_generator<Derived, EngineType>::discard(unsigned long long num)
{
	matrix_t J = JumpMatrix(num);

	state s = getState();

	// advance num steps
	s = J * s;

	// correct the initial r bits of the state
	Derived::correct(s);

	// upcast to seed
	static_cast<Derived *>(this) -> seed(s);
}

// peek
template<typename Derived, typename EngineTraits>
inline typename linear_generator<Derived, EngineTraits>::result_type 
linear_generator<Derived, EngineTraits>::peek(unsigned long long v) const
{
	result_type value;

	TransformedGet0(&value, 1, v);

	return value;
}

/* member classes */

//! the state of a linear generator
template<typename Derived, typename EngineTraits>
struct linear_generator<Derived, EngineTraits>::state
{	
    //QFCL_USING_TYPE(UIntType, (linear_generator<Derived, EngineTraits>));
    //using linear_generator<Derived, EngineTraits>::n;
    //using linear_generator<Derived, EngineTraits>::mod;
    //static const size_t n	= EngineTraits::state_size;
    //static const size_t mod = EngineTraits::modulus;
    //typedef typename EngineTraits::UIntType UIntType;

	//! constructor taking an array of \c UIntType of size \c n, representing the state
	state(const UIntType (&arr)[n]) {set(arr);}
	//! constructor taking the internal state of a \c linear_generator
	state(const UIntType (&x)[n], size_t i) {set(x, i);}
	//! takes a vector_t representing the state
	state(const Vector<mod> & arr);
	//! assignment from vector_t
	const state & operator=(const Vector<mod> & arr) 
	{
		return *this = static_cast<state>(arr);
	}
	//! set the state from an array of \c UIntType of size \c n, representing the state
	void set(const UIntType (&arr)[n]) {std::copy(&arr[0], &arr[n], s);}
	//! set the state from the internal state of a \c linear_generator
	void set(const UIntType (&x)[n], size_t i);
	//! get the representation as a state array
	const UIntType * rep() const {return s;}
	//! the number of \c UIntType elements comprising the state
	static const size_t length() {return n;}

	//! conversion function from state to vector_t<mod>
	// compiler has problem with out-of-class definition
	operator Vector<mod>()
	{
		using namespace qfcl::math;

		Vector<mod> a;
		a.SetLength(k);

		// I am assuming that the standard specifices that the 1 byte storage of char
		// contains exactly 8 bits of information
		// See also the implicit conversion from Vector<mod> to state.
		static const size_t ntl_w = sizeof(*a.rep.rep) * 8;
		copy_bits( make_bit_pseudoiterator<w>(&s[0], r), 
				   make_bit_pseudoiterator<w>(&s[n], 0),
				   make_bit_pseudoiterator<ntl_w>(a.rep.rep, 0) );

		return a;
	}
private:
	UIntType s[n];
};

// set
template<typename Derived, typename EngineTraits>
inline void
linear_generator<Derived, EngineTraits>::state::set(const UIntType (&x)[n], size_t i)
{
	linear_generator<Derived, EngineTraits>::Get(x, i, &s[0], n);
}

// ctor
template<typename Derived, typename EngineTraits>
linear_generator<Derived, EngineTraits>::state::state(const Vector<mod> & arr)
{
	using namespace qfcl::math;

	// NTL internal representation uses the *first* k bits, 
	// so that only the lower w - r bits of arr.rep.rep[n - 1] are used, 
	// whereas we use the *last* k bits so that only the upper w - r
	// bits of s[0] are used.
	static const size_t ntl_w = sizeof(*arr.rep.rep) * 8;
	// r may be greater than ntl_w, and we have to accomodate for r == 0 anyways
	static const size_t ntl_shift_bits = (ntl_w - r) % ntl_w; 
	static const size_t ntl_shift_words = (r + ntl_w - 1) / ntl_w; 
	copy_bits( make_bit_pseudoiterator<ntl_w>(arr.rep.rep, 0),
			   make_bit_pseudoiterator<ntl_w>( arr.rep.rep + arr.rep.length() - ntl_shift_words, ntl_shift_bits ),
			   make_bit_pseudoiterator<w>(s, r), true );
}

/* private static data */

template<typename Derived, typename EngineTraits>
typename linear_generator<Derived, EngineTraits>::matrix_t 
linear_generator<Derived, EngineTraits>::J;

template<typename Derived, typename EngineTraits>
bool linear_generator<Derived, EngineTraits>::J_set = false;

template<typename Derived, typename EngineTraits>
unsigned long long linear_generator<Derived, EngineTraits>::computed_size;

template<typename Derived, typename EngineTraits>
bool linear_generator<Derived, EngineTraits>::computed_reverse;

/* private static member functors */

// Get
template<typename Derived, typename EngineTraits>
template<typename OutIt>
inline void
linear_generator<Derived, EngineTraits>::Get(const UIntType (&x)[n], size_t i, OutIt dest, size_t num, unsigned long long skip)
{
	for (; i < n - 1 && skip > 0; ++i, --skip);

	for (; i < n - 1 && num > 0; ++i, --num)
		*(dest++) = Derived::GetNext(x, i);

	UIntType y[n];
	i = n - 1;
	for (; skip > 0; --skip)
		Derived::GetNextState(y, i, x);

	for (; num > 0; --num)
		*(dest++) = Derived::GetNextState(y, i, x);
}

// TransitionMatrix_imp
template<typename Derived, typename EngineType>
inline typename linear_generator<Derived, EngineType>::matrix_t & 
linear_generator<Derived, EngineType>::TransitionMatrix_imp(bool reverse)
{
	// transition matrix
	static matrix_t A;
	static bool A_set = false;

	// reverse transition matrix
	static matrix_t A_reverse;
	static bool A_reverse_set = false;

	if (!reverse && A_set)
		return A;
	else if (reverse && A_reverse_set)
		return A_reverse;

	if (!reverse)
	{
     //   A = linear_generator<Derived, EngineType>::obtain_matrix( transition_matrix_filename(), transition_matrix_functor() );
        A = obtain_matrix( transition_matrix_filename(), transition_matrix_functor() );
		A_set = true;
		return A;
	}
	else
	{
		A_reverse = obtain_matrix( transition_matrix_filename(reverse), transition_matrix_functor(reverse) );
		A_reverse_set = true;
		return A_reverse;
	}
}

// TransitionMatrix_file_exists_imp
template<typename Derived, typename EngineTraits>
bool 
linear_generator<Derived, EngineTraits>::TransitionMatrix_file_exists_imp(bool reverse)
{
	std::string filename = transition_matrix_filename(reverse);

	return std::ifstream( filename.c_str() );
}

// JumpMatrix_imp
template<typename Derived, typename EngineTraits>
inline typename linear_generator<Derived, EngineTraits>::matrix_t & 
linear_generator<Derived, EngineTraits>::JumpMatrix_imp(unsigned long long jump_size, bool reverse)
{
	// -0 = 0
	reverse = (reverse && jump_size > 0);

	// check whether J currently holds the matrix for this jump, in both size and direction
	if ( J_set && jump_size == computed_size && computed_reverse == reverse )
		return J;

	// filename of the jump matrix not reversed
	std::string filename = jump_matrix_filename(jump_size);
	
	if (reverse)
	{
		std::string reverse_filename = jump_matrix_filename(jump_size, reverse);
		J = obtain_matrix( reverse_filename, reverse_jump_matrix_functor(jump_size, filename) );
	}
	else
		J = obtain_matrix( filename, jump_matrix_functor(jump_size) );

	J_set = true;
	computed_size = jump_size;
	computed_reverse = reverse;

	return J;
}

// JumpMatrix_file_exists_imp
template<typename Derived, typename EngineTraits>
bool 
linear_generator<Derived, EngineTraits>::JumpMatrix_file_exists_imp(unsigned long long jump_size, bool reverse)
{
	std::string filename = jump_matrix_filename(jump_size, reverse);

	return std::ifstream( filename.c_str() );
}

// obtain_matrix
template<typename Derived, typename EngineTraits>
template<typename F>
typename linear_generator<Derived, EngineTraits>::matrix_t 
linear_generator<Derived, EngineTraits>::obtain_matrix(const std::string & filename, const F & f)
{
	matrix_t A;

	// read from file if available
	try
	{
		bool exists = A.read(filename);
		if (!exists)
		{
			A = f();
			A.write(filename);
		}
	}
	catch(std::exception e)
	{
		std::cerr << "Exception thrown: ";
		std::cerr << e.what() << std::endl;
		exit(EXIT_FAILURE);
	}

	return A;
}

// struct transition_matrix_functor
template<typename Derived, typename EngineTraits>
struct linear_generator<Derived, EngineTraits>::transition_matrix_functor
{
	transition_matrix_functor(bool _reverse = false) : reverse(_reverse) {}

    matrix_t operator()() const
	{
		matrix_t A;

		// compute A
		A.SetDims(k, k);
		for (size_t i = 0; i < k; ++i)
		{
			vector_t x;
			x.SetLength(k);
			x[i] = 1;

	
			// i-th column of A

			// work with A^T instead
			// note that, as a concept feature, we need a corrected state when going in reverse
			A[i] = !reverse ? Derived::Transition(x) 
				: Derived::ReverseTransition( invertible_linear_generator<Derived, EngineTraits>::correct(x) );
		}

		return transpose(A);
	}
private:
	const bool reverse;
};

// struct jump_matrix_functor
template<typename Derived, typename EngineTraits>
struct linear_generator<Derived, EngineTraits>::jump_matrix_functor
{
	jump_matrix_functor(unsigned long long _jump_size) : jump_size(_jump_size) {}

    matrix_t operator()() const
	{
		//return NTL::power( TransitionMatrix(), jump_size );
		return detail::pow( TransitionMatrix_imp(false), jump_size );
	}
private:
    const unsigned long long jump_size;
};

// struct reverse_jump_matrix_functor
template<typename Derived, typename EngineTraits>
struct linear_generator<Derived, EngineTraits>::reverse_jump_matrix_functor
{
	reverse_jump_matrix_functor(unsigned long long _jump_size, const std::string & _fwd_filename)
		: jump_size(_jump_size), fwd_filename(_fwd_filename) {}

    matrix_t operator()() const
	{
		//matrix_t fwd_J = obtain_matrix( fwd_filename, jump_matrix_functor(jump_size) );
		//return NTL::inv(fwd_J);
		return detail::pow( TransitionMatrix_imp(true), jump_size );
	}
private:
    const unsigned long long jump_size;
	std::string fwd_filename;
};

// transition_matrix_filename
template<typename Derived, typename EngineTraits>
std::string 
linear_generator<Derived, EngineTraits>::transition_matrix_filename(bool reverse)
{
	static const std::string type_id = "TransitionMatrix";
    static const std::string engine_name = boost::mpl::c_str<typename Derived::name>::value;
	std::string filename;

	if (!reverse)
		filename = engine_name + '.' + type_id;
	else
		filename = engine_name + '.' + type_id + "Inverse";

	return filename;
}

// jump_matrix_filename
template<typename Derived, typename EngineTraits>
std::string 
linear_generator<Derived, EngineTraits>::jump_matrix_filename(unsigned long long jump_size, bool reverse)
{
	static const std::string type_id = "JumpMatrix";
    static const std::string engine_name = boost::mpl::c_str<typename Derived::name>::value;
	std::string filename;

	if (!reverse)
	{
		filename = engine_name + '.' + type_id 
			+ '.' + boost::lexical_cast<std::string>(jump_size);
	}
	else
	{
		filename = engine_name + '.' + type_id
			+ '.' + '-' + boost::lexical_cast<std::string>(jump_size);
	}

	return filename;
}

// equal_imp
template<typename Derived, typename EngineTraits>
bool 
linear_generator<Derived, EngineTraits>::equal_imp(const linear_generator & eng) const
{
	const size_t diff = eng.i - i;	// this is always called with eng.i >= i

	// we have immediate access to the previous i numbers generated by *this
	// and we compare these with eng
	for (size_t j = 0; j < i; ++j)
		if (x[j] != eng.x[j + diff])
			return false;

	// we have immediate access to the next n - eng.i generated by eng
	// and we compare these with *this
	for (size_t j = eng.i; j < n; ++j)
		if (x[j - diff] != eng.x[j])
			return false;

	// we have immediate access to the j-th number generated by *this, for j in [n-eng.i,n-i),
	// and compare with the next diff numbers to be generated by eng
	if (diff != 0)
	{
		UIntType arr[n];

		Get(eng.x, eng.i, arr, diff);
		if ( !std::equal(arr, arr + diff, &x[i + n - eng.i]) )
			return false;
	}

	// the generators have a common sequence of length n
	return true;
}

template<typename Derived, typename EngineTraits>
template<typename OutIt>
inline void
linear_generator<Derived, EngineTraits>::TransformedGet0(OutIt dest, size_t num, unsigned long long skip) const
{
	using namespace std;
	UIntType y[n];
	copy( begin(x), end(x), begin(y) );
	size_t j = i;

	for (; skip > 0; --skip)
		Derived::Next(y, j);

	while (num > 0)
	{
		*(dest++) = Derived::Transform(y, j);
		if (--num > 0)
			Derived::Next(y, j);
	}
}

/** noninvertible_linear_generator class template */

template<typename Derived, typename EngineTraits>
class noninvertible_linear_generator : public linear_generator<Derived, EngineTraits>
{
	typedef linear_generator<Derived, EngineTraits> base_type;
public:
	// ctor
	/*! \sa seed(UIntType)
	*/
    noninvertible_linear_generator(typename base_type::UIntType seed_ = base_type::default_seed) {seed(seed_);}

	//! re-seed the generator, \c seed() resets to the default seed
    void seed(typename base_type::UIntType seed_ = base_type::default_seed) {seed_imp(seed_);}
	//! re-seed the generator with a sequence of seeds of arbitrary length
	template<typename It>
    void seed(It begin, It end) {this -> seed_imp(begin, end);}
	//! set the generator by directly providing the state
    void seed(typename base_type::state & s) {seed_imp(s);}
	//! generate a random number
    typename base_type::result_type operator()()
    {
        auto z = Derived::Transform(this -> x, this -> i);
        Derived::Next(this -> x, this -> i);
        return z;
    }
};

/** invertible_linear_generator class template */

template<typename Derived, typename EngineTraits>
class invertible_linear_generator : public linear_generator<Derived, EngineTraits>
{
/// NOTE: Why can't this be private?
//	typedef linear_generator<Derived, EngineTraits> base_type;
public:
    typedef linear_generator<Derived, EngineTraits> base_type;

    // inherit from base
    QFCL_USING_TYPE(result_type, base_type);
    QFCL_USING_TYPE(UIntType, base_type);
    QFCL_USING_TYPE(state, base_type);
    QFCL_USING_TYPE(matrix_t, base_type);
    using base_type::default_seed;
    using base_type::n;

    // ctor
	/*! \sa seed(UIntType)
	*/
    invertible_linear_generator(UIntType seed_ = default_seed) {seed(seed_);}

	//! re-seed the generator, \c seed() resets to the default seed
    void seed(UIntType seed_ = default_seed);
	//! re-seed the generator with a sequence of seeds of arbitrary length
	template<typename It>
	void seed(It begin, It end);
	//! set the generator by directly providing the state \p s
    void seed(const state & s);//, bool auto_correct = false);

	//! generate a random number
    result_type operator()()
    {
        Derived::Next(this -> x, this -> i);
        return Derived::Transform(this -> x, this -> i);
    }

	/*! \brief generate a random number in reverse
	
		Returns the previously generated random number, 
		and sets the generator to the state prior
		to the generation of this number.
	*/
    result_type reverse()
    {
        result_type z = Derived::Transform(this -> x, this -> i);
        Derived::Previous(this -> x, this -> i);
        return z;
    }
	//! correct the "unused" bits of the state
	static state & correct(state & s);
	static state correct(const state & s);
	
	//! revert the state by \c num steps
	void reverse_discard(unsigned long long num);
	//! skip \c num step, where \c num can be negative
	void skip(long long num);
	
	//! get the next random number to be generated (without changing the state)
	result_type peek(long long v) const;
	result_type reverse_peek(unsigned long long v) const;

	//! TransitionMatrix and its inverse
	static matrix_t & TransitionMatrix(bool reverse = false)
	{
        return base_type::TransitionMatrix_imp(reverse);
	}
	//! whether the corresponding matrix exists
	static bool TransitionMatrix_file_exists(bool reverse = false)
	{
        return base_type::TransitionMatrix_file_exists_imp(reverse);
	}

	//! JumpMatrices for negative jumps too
	static matrix_t & JumpMatrix(unsigned long long jump_size, bool reverse = false)
	{
        return base_type::JumpMatrix_imp(jump_size, reverse);
	}
	static matrix_t & JumpMatrix(long long jump_size) {return JumpMatrix( ::abs(jump_size), jump_size < 0 );}
	
	//! whether the file containing the jump matrix of jump size v exists
	static bool JumpMatrix_file_exists(unsigned long long jump_size, bool reverse = false)
	{
        return base_type::JumpMatrix_file_exists_imp(jump_size, reverse);
	}
	static bool JumpMatrix_file_exists(long long jump_size) {return JumpMatrix_file_exists( ::abs(jump_size), jump_size < 0 );}
private:
	//! reverse version of TransformedGet0
	template<typename OutIt>
	void ReverseTransformedGet0(OutIt dest, size_t num, unsigned long long skip = 0) const;
};

// seed
/*! Only the last \c w bits of each seed are used.
*/
template<typename Derived, typename EngineType>
inline void
invertible_linear_generator<Derived, EngineType>::seed(UIntType seed)
{
    this -> seed_imp(seed);

	// back up one step, so that s gives the *next* n numbers
	// this also corrects the lower r bits of x[0]
    Derived::Previous(this -> x, this -> i);
}

// seed
/*! Only the last \c w bits of each seed are used.
*/
template<typename Derived, typename EngineType>
template<typename It>
inline void
invertible_linear_generator<Derived, EngineType>::seed(It begin, It end)
{
	seed_imp(begin, end);

	// back up one step, so that s gives the *next* n numbers
	// this also corrects the lower r bits of x[0]
    Derived::Previous(this -> x, this -> i);
}

// seed
/*! Only the last \c w bits of each seed are used.

	If \p auto_correct is \c true, then the extra bits in \p s are 
	automatically corrected, making it a valid state; otherwise,
	throws a \c domain_error exception.
*/
template<typename Derived, typename EngineType>
inline void
invertible_linear_generator<Derived, EngineType>::seed(const state & s)
{
	// verify that the state is consistent
	// this is done by going forward one step, and then computing x[0] by moving back from x[1]
    std::copy( s.rep(), s.rep() + n, this -> x );
    this -> i = n - 1;
    Derived::GetNextState(this -> x, this -> i);
    Derived::GetPreviousState(this -> x, this -> i);

    if (this -> x[0] != s.rep()[0])
		throw std::domain_error("invalid state given for the invertible linear generator");

    this -> seed_imp(s);

	// back up one step, so that s gives the *next* n numbers
    Derived::Previous(this -> x, this -> i);
}

// correct
template<typename Derived, typename EngineType>
typename invertible_linear_generator<Derived, EngineType>::state &
invertible_linear_generator<Derived, EngineType>::correct(state & s)
{
    UIntType y[n];
    std::copy( s.rep(), s.rep() + n, y );
    size_t j = n - 1;
    Derived::GetNextState(y, j);
    Derived::GetPreviousState(y, j);
    s.set(y);

	return s;
}

// correct
template<typename Derived, typename EngineType>
typename invertible_linear_generator<Derived, EngineType>::state
invertible_linear_generator<Derived, EngineType>::correct(const state & s)
{
	state r = s;

	correct(r);

	return r;
}

// reverse_discard
template<typename Derived, typename EngineType>
inline void
invertible_linear_generator<Derived, EngineType>::reverse_discard(unsigned long long num)
{
	matrix_t J = JumpMatrix(num, true);

	/// NOTE: FACTOR OUT COMMON CODE. FIX THIS
    state s = this -> getState();

	// advance num steps
	s = J * s;

	// correct the initial r bits of the state
	Derived::correct(s);

	// upcast to seed
	static_cast<Derived *>(this) -> seed(s);
}

// skip
template<typename Derived, typename EngineTraits>
inline void
invertible_linear_generator<Derived, EngineTraits>::skip(long long num)
{
    num >= 0 ? this -> discard(num) : this -> reverse_discard(-num);
}

// reverse_peek
template<typename Derived, typename EngineTraits>
inline typename invertible_linear_generator<Derived, EngineTraits>::result_type
invertible_linear_generator<Derived, EngineTraits>::reverse_peek(unsigned long long v) const
{
	result_type value;

	ReverseTransformedGet0(&value, 1, v);

	return value;
}

// peek
template<typename Derived, typename EngineTraits>
inline typename invertible_linear_generator<Derived, EngineTraits>::result_type 
invertible_linear_generator<Derived, EngineTraits>::peek(long long v) const
{
	result_type value;

    v >= 0 ? this -> TransformedGet0(&value, 1, v) : ReverseTransformedGet0(&value, 1, -v);

	return value;
}

/* private member function */

template<typename Derived, typename EngineTraits>
template<typename OutIt>
inline void
invertible_linear_generator<Derived, EngineTraits>::ReverseTransformedGet0(OutIt dest, size_t num, unsigned long long skip) const
{
	using namespace std;
	UIntType y[n];
    copy( begin(this -> x), end(this -> x), begin(y) );
    size_t j = this -> i;

	// recall that we begin with the random number just generated
//	if (skip == 0)
//		Derived::Next(y, j);
	for (; skip > 0; --skip)
		Derived::Previous(y, j);

	while(num > 0)
	{
		*(dest++) = Derived::Transform(y, j);
		if (--num > 0)
			Derived::Previous(y, j);
	}
}

/** linear_generator traits classes */

template<typename LG>
struct linear_generator_traits
{
	static const bool is_TGFSR = LG::mask_bits == 0;
};

/** linear_generator policy traits */



//! @}

}	// namespace random

}	// namespace qfcl

#endif	// QFCL_RANDOM_LINEAR_GENERATOR_HPP
