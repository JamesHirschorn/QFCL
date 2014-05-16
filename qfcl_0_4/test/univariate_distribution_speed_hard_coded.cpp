/* test/univariate_generator_speed_hard_coded.cpp
 *
 * Copyright (C) 2014 James Hirschorn <James.Hirschorn@gmail.com>
 *
 * Use, modification and distribution are subject to 
 * the BOOST Software License, Version 1.0. 
 * (See accompanying file LICENSE.txt)
 */

/*! \file test/univariate_generator_speed_hard_coded.cpp
	\brief hard-coded version of univariate generation speed test

	\author James Hirschorn
	\date February 23, 2014
*/

#include <iostream>
#include <stdexcept>
#include <string>
using std::string;
#include <vector>
using std::vector;

#include <boost/cstdint.hpp>
#include <boost/foreach.hpp>
#include <boost/format.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/mpl/push_front.hpp>
namespace mpl = boost::mpl;
#include <boost/random.hpp>
#include <boost/timer/timer.hpp>

#include <qfcl/defines.hpp>
#include <qfcl/random/engine/mersenne_twister.hpp>
#include <qfcl/utility/comma_separated_number.hpp>
#include <qfcl/utility/names.hpp>
#include <qfcl/utility/tmp.hpp>
#include <qfcl/utility/type_selection.hpp>

#include <qfcl/random/distribution/normal_box_muller.hpp>
#include <qfcl/random/distribution/normal_box_muller_polar.hpp>
#include <qfcl/random/distribution/QuantLib_normal_box_muller_polar.hpp>

#include "utility/cpu_timer.hpp"

/** Select which generators to test. */
#define UNIFORM_0IN_1EX
//#define UNIFORM_0IN_1IN
//#define NORMAL_BOX_MULLER_POLAR

// which TSC to use

#ifdef QFCL_RDTSCP
typedef qfcl::timer::rdtscp_timer timer_t;
#else
typedef qfcl::timer::rdtsc_timer timer_t;
#endif

// output results

template<typename CounterType>
void show_timing_results(uint64_t clock_cycles, CounterType iterations, const string & distribution_name, const string & variate_method, double cpu_freq)
{
	// time taken in seconds
	double time_taken = clock_cycles / cpu_freq;

	std::cout 
		<< boost::format("%1%.\nMethod: %2%\n%|3$18.2f| random numbers/second = %|4$13.8f| nanoseconds/random number = %|5$6.1f| CPU cycles/random number\n\n")
			% distribution_name % variate_method
			% ( iterations / time_taken ) 
			% ( time_taken * UINT64_C(1000000000) / iterations ) 
			% ( double(clock_cycles) / iterations);
}

int main(int argc, char * argv[])
{
	using namespace std;
	using namespace qfcl::random;

	typedef unsigned long long CounterType;
	//typedef unsigned long CounterType;

	CounterType iterations = 100000000;

	bool show_values = false;

	typedef mt19937 Engine;
	//typedef boost::random::mt19937 Engine;
		
#ifndef	SYSTEM_CPU_FREQUENCY
    double cpu_frequency = qfcl::timer::get_cpu_frequency();
#else
	double cpu_frequency = SYSTEM_CPU_FREQUENCY;
#endif	// SYSTEM_CPU_FREQUENCY

    cout << boost::format("CPU frequency: %|1$.5| GHz\n\n") % (cpu_frequency / qfcl::timer::one_second);

	cout << qfcl::io::custom_formatted(iterations) << " iterations per engine:" << endl << endl;

#ifdef UNIFORM_0IN_1EX
	{
		typedef uniform_0in_1ex<> Distribution;
		Engine e;
		Distribution d;
	
		volatile Distribution::result_type value;
												
		timer_t timer;								
												
		uint64_t start = timer();					
												
		for (CounterType i = 0; i < iterations; ++i)		
		{											
			value = d(e);		
			if (show_values)
			{
				cout << boost::format("Random number %|1$3|: %|2$20.10|.\n") % i % value; 
			}
		}											
												
		uint64_t end = timer();						
												
		uint64_t result = end - start;	

		show_timing_results(
			result, iterations, 
			qfcl::names::name_or_typename(d), qfcl::names::name(Distribution::method()), 
			cpu_frequency);
	}
#endif
#ifdef NORMAL_BOX_MULLER
	{
		typedef normal_box_muller<> Distribution;
		Engine e;
		Distribution d;
	
		volatile Distribution::result_type value;
												
		timer_t timer;								
												
		uint64_t start = timer();					
												
		for (CounterType i = 0; i < iterations; ++i)		
		{											
			value = d(e);		
			if (show_values)
			{
				cout << boost::format("Random number %|1$3|: %|2$20.10|.\n") % i % value; 
			}
		}											
												
		uint64_t end = timer();						
												
		uint64_t result = end - start;	

		show_timing_results(
			result, iterations, 
			qfcl::names::name_or_typename(d), qfcl::names::name(Distribution::method()), 
			cpu_frequency);
	}
#endif 
#ifdef NORMAL_BOX_MULLER_POLAR
	{
		typedef normal_box_muller_polar<> Distribution;
		Engine e;
		Distribution d;
	
		volatile Distribution::result_type value;
												
		timer_t timer;								
												
		uint64_t start = timer();					
												
		for (CounterType i = 0; i < iterations; ++i)		
		{											
			value = d(e);		
			if (show_values)
			{
				cout << boost::format("Random number %|1$3|: %|2$20.10|.\n") % i % value; 
			}
		}											
												
		uint64_t end = timer();						
												
		uint64_t result = end - start;	

		show_timing_results(
			result, iterations, 
			qfcl::names::name_or_typename(d), qfcl::names::name(Distribution::method()), 
			cpu_frequency);
	}	
#endif
#ifdef QUANTLIB_NORMAL_BOX_MULLER
	{
		typedef  QuantLib_normal_box_muller_polar<> Distribution;
		Engine e;
		Distribution d;
	
		volatile Distribution::result_type value;
												
		timer_t timer;								
												
		uint64_t start = timer();					
												
		for (CounterType i = 0; i < iterations; ++i)		
		{											
			value = d(e);							
			if (show_values)
			{
				cout << boost::format("Random number %|1$3|: %|2$20.10|.\n") % i % value; 
			}
		}											
												
		uint64_t end = timer();						
												
		uint64_t result = end - start;	

		show_timing_results(
			result, iterations, 
			qfcl::names::name_or_typename(d), qfcl::names::name(Distribution::method()), 
			cpu_frequency);
	}
#endif

	cout << "Press Enter to exit.";

	char c;
	cin.get(c);
}
