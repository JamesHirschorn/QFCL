/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2003 Ferdinando Ametrano
 Copyright (C) 2000, 2001, 2002, 2003 RiskMap srl

 Modified by James Hirschorn, February 20, 2014.

 This file is part of QuantLib, a free-software/open-source library
 for financial quantitative analysts and developers - http://quantlib.org/

 QuantLib is free software: you can redistribute it and/or modify it
 under the terms of the QuantLib license.  You should have received a
 copy of the license along with this program; if not, please email
 <quantlib-dev@lists.sf.net>. The license is also available online at
 <http://quantlib.org/license.shtml>.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the license for more details.
*/

/*! \file boxmullergaussianrng.hpp
    \brief Box-Muller Gaussian random-number generator
*/

#ifndef QFCL_RANDOM_GENERATOR_QUANTLIB_BOX_MULLER_GAUSSIAN_RNG_HPP
#define QFCL_RANDOM_GENERATOR_QUANTLIB_BOX_MULLER_GAUSSIAN_RNG_HPP

#include <tuple>

#include <boost/type_traits/is_pointer.hpp>
#include <boost/type_traits/remove_pointer.hpp>
#include <boost/utility/enable_if.hpp>

#include <ql/methods/montecarlo/sample.hpp>

namespace qfcl {
namespace QuantLib {
    //! Gaussian random number generator
    /*! It uses the well-known Box-Muller transformation to return a
        normal distributed Gaussian deviate with average 0.0 and
        standard deviation of 1.0, from a uniform deviate in (0,1)
        supplied by RNG.

        Class RNG must implement the following interface:
        \code
            RNG::sample_type RNG::next() const;
        \endcode
    */
    template <typename Derived, typename RNG, typename RealType>
    class BoxMullerGaussianRng_base {
      public:
        typedef ::QuantLib::Sample<RealType> sample_type;
        typedef RNG urng_type;
        explicit BoxMullerGaussianRng_base(RNG const& uniformGenerator);
        //! returns a sample from a Gaussian distribution
        sample_type next() const;
      protected:
        RNG uniformGenerator_;
        mutable bool returnFirst_;
        mutable RealType firstValue_,secondValue_;
        mutable RealType firstWeight_,secondWeight_;
        mutable RealType weight_;
	};
	
    template <typename Derived, typename RNG, typename RealType>
    BoxMullerGaussianRng_base<Derived, RNG, RealType>::BoxMullerGaussianRng_base(RNG const& uniformGenerator)
    : uniformGenerator_(uniformGenerator), returnFirst_(true),
      weight_(0.0) {}

    template <typename Derived, typename RNG, typename RealType>
    inline typename BoxMullerGaussianRng_base<Derived, RNG, RealType>::sample_type
    BoxMullerGaussianRng_base<Derived, RNG, RealType>::next() const {
        if (returnFirst_) {
            RealType x1,x2,r,ratio;
			std::tie(x1, x2, r, ratio) = static_cast<Derived const* const>(this) -> _computation();

            ratio = std::sqrt(-2.0*std::log(r)/r);
            firstValue_ = x1*ratio;
            secondValue_ = x2*ratio;
            weight_ = firstWeight_*secondWeight_;

            returnFirst_ = false;
            return sample_type(firstValue_,weight_);
        } else {
            returnFirst_ = true;
            return sample_type(secondValue_,weight_);
        }
    }

	template<typename RNG, typename Real, typename Enable = void>
	class BoxMullerGaussianRng
	{};

	template<typename RNG, typename Real>
	class BoxMullerGaussianRng<RNG, Real, typename boost::disable_if<boost::is_pointer<RNG>>::type>
		: public BoxMullerGaussianRng_base<
			  BoxMullerGaussianRng<RNG, Real, typename boost::disable_if<boost::is_pointer<RNG>>::type>
			, RNG
			, Real
			>
	{
		typedef BoxMullerGaussianRng_base<
				  BoxMullerGaussianRng<RNG, Real, typename boost::disable_if<boost::is_pointer<RNG>>::type>
				, RNG
				, Real
				>
		base_type;      
        using base_type::uniformGenerator_;
        using base_type::firstWeight_;
        using base_type::secondWeight_;
	public:
  		explicit BoxMullerGaussianRng(RNG const& uniformGenerator)
			:  base_type(uniformGenerator)
		{}
		friend base_type;
	private:
		//using base_type::uniformGenerator_;
		std::tuple<Real, Real, Real, Real> 
		_computation() const;
    };

	template<typename RNG, typename Real>
	inline std::tuple<Real, Real, Real, Real> 
	BoxMullerGaussianRng<RNG, Real, typename boost::disable_if<boost::is_pointer<RNG>>::type>
		::_computation() const
	{
		Real x1,x2,r,ratio;
		do {
			base_type::uniformGenerator_.next();
			typename RNG::sample_type s1 = base_type::uniformGenerator_.next();
			x1 = s1.value*2.0-1.0;
			firstWeight_ = s1.weight;
			typename RNG::sample_type s2 = uniformGenerator_.next();
			x2 = s2.value*2.0-1.0;
			secondWeight_ = s2.weight;
			r = x1*x1+x2*x2;
		} while (r>=1.0 || r==0.0);

		return std::make_tuple(x1, x2, r, ratio);
	}


	template<typename RNG, typename Real>
	class BoxMullerGaussianRng<RNG, Real, typename boost::enable_if<boost::is_pointer<RNG>>::type>
		: public BoxMullerGaussianRng_base<
			  BoxMullerGaussianRng<RNG, Real, typename boost::enable_if<boost::is_pointer<RNG>>::type>
			, RNG
			, Real
			>
	{
		typedef BoxMullerGaussianRng_base<
			  BoxMullerGaussianRng<RNG, Real, typename boost::enable_if<boost::is_pointer<RNG>>::type>
			, RNG
			, Real
			> base_type;
        using base_type::uniformGenerator_;
        using base_type::firstWeight_;
        using base_type::secondWeight_;
	public:
		explicit BoxMullerGaussianRng(RNG const& uniformGenerator)
			:  BoxMullerGaussianRng_base<BoxMullerGaussianRng, RNG, Real>(uniformGenerator)
		{}
		friend base_type;
	private:
		std::tuple<Real, Real, Real, Real> 
		_computation() const;
    };

	template<typename RNG, typename Real>
	inline std::tuple<Real, Real, Real, Real> 
	BoxMullerGaussianRng<RNG, Real, typename boost::enable_if<boost::is_pointer<RNG>>::type>
		::_computation() const
	{
		Real x1,x2,r,ratio;
		do {
			typename boost::remove_pointer<RNG>::type::sample_type s1 = uniformGenerator_ -> next();
			x1 = s1.value*2.0-1.0;
			firstWeight_ = s1.weight;
			typename boost::remove_pointer<RNG>::type::sample_type s2 = uniformGenerator_ -> next();
			x2 = s2.value*2.0-1.0;
			secondWeight_ = s2.weight;
			r = x1*x1+x2*x2;
		} while (r>=1.0 || r==0.0);

		return std::make_tuple(x1, x2, r, ratio);
	}

}}	// namespace qfcl::QuantLib


#endif	!QFCL_RANDOM_GENERATOR_QUANTLIB_BOX_MULLER_GAUSSIAN_RNG_HPP
