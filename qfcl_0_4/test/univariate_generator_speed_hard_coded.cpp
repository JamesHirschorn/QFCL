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
#include <boost/random.hpp>
#include <boost/timer/timer.hpp>

#include <ql/experimental/math/zigguratrng.hpp>
#include <ql/math/randomnumbers/mt19937uniformrng.hpp>

#include <qfcl/defines.hpp>
#include <qfcl/random/engine/mersenne_twister.hpp>
#include <qfcl/utility/comma_separated_number.hpp>
#include <qfcl/utility/names.hpp>
#include <qfcl/utility/tmp.hpp>
#include <qfcl/utility/type_selection.hpp>

//#include <qfcl/random/distribution/normal_box_muller.hpp>
#include <qfcl/random/distribution/boost_normal_distribution.hpp>
#include <qfcl/random/distribution/boost_normal_ziggurat.hpp>
#include <qfcl/random/distribution/normal_box_muller_polar.hpp>
#include <qfcl/random/distribution/QuantLib_normal_box_muller_polar.hpp>
#include <qfcl/random/distribution/uniform_0in_1ex.hpp>
#include <qfcl/random/distribution/uniform_0in_1in.hpp>
#include <qfcl/random/generator/QuantLib_BoxMullerGaussianRng.hpp>
#include <qfcl/random/generator/QuantLib_ZigguratRng.hpp>

#include "utility/cpu_timer.hpp"

/** Select which generators to test. */
//#define UNIFORM_0IN_1EX
//#define UNIFORM_0IN_1IN
//#define QUANTLIB_MT19937_UNIFORM_RNG	
//#define BOOST_NORMAL_BOX_MULLER
//#define NORMAL_BOX_MULLER_POLAR
//#define BOOST_NORMAL_BOX_MULLER_POLAR	// uses boost::variate_generator with qfcl::random::normal_box_muller_polar
//#define QUANTLIB_NORMAL_BOX_MULLER_POLAR
//#define QUANTLIB_NATIVE_NORMAL_BOX_MULLER_POLAR
//#define QUANTLIB_WRAPPED_NATIVE_NORMAL_BOX_MULLER_POLAR
#define BOOST_NORMAL_ZIGGURAT
#define QUANTLIB_NATIVE_NORMAL_ZIGGURAT
#define QUANTLIB_NORMAL_ZIGGURAT

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

	//typedef unsigned long long CounterType;
	typedef unsigned long CounterType;

	CounterType iterations = 100000000;
	//CounterType iterations = 1;

//#define SHOW_VALUES

	typedef qfcl::random::mt19937 Engine;
	//typedef boost::random::mt19937 Engine;
		
#ifndef	SYSTEM_CPU_FREQUENCY
    double cpu_frequency = qfcl::timer::get_cpu_frequency();
#else
	double cpu_frequency = SYSTEM_CPU_FREQUENCY;
#endif	// SYSTEM_CPU_FREQUENCY

    cout << boost::format("CPU frequency: %|1$.5| GHz\n\n") % (cpu_frequency / qfcl::timer::one_second);

	cout << qfcl::io::custom_formatted(iterations) << " iterations per engine:" << endl << endl;

	// uniform_0in_1ex
#ifdef UNIFORM_0IN_1EX
	{
		typedef qfcl::random::uniform_0in_1ex<> Distribution;

		typedef qfcl::random::variate_generator<Engine, Distribution> Generator;
		Engine e;
		Distribution d;
		Generator gen(e, d);
	
		volatile Generator::result_type value;
												
		timer_t timer;								
												
		uint64_t start = timer();					
												
		for (CounterType i = 0; i < iterations; ++i)		
		{											
			value = gen();		
#ifdef SHOW_VALUES
				cout << boost::format("Random variate %|1$3|: %|2$20.10|.\n") % i % value; 
#endif
		}											
												
		uint64_t end = timer();						
												
		uint64_t result = end - start;	

		show_timing_results(
			result, iterations, 
			qfcl::names::name_or_typename(gen), qfcl::names::name_or_typename(Distribution::method()), 
			cpu_frequency);
	}
#endif
#ifdef UNIFORM_0IN_1IN
	{
		typedef qfcl::random::uniform_0in_1in<> Distribution;

		typedef qfcl::random::variate_generator<Engine, Distribution> Generator;
		Engine e;
		Distribution d;
		Generator gen(e, d);
	
		volatile Generator::result_type value;
												
		timer_t timer;								
												
		uint64_t start = timer();					
												
		for (CounterType i = 0; i < iterations; ++i)		
		{											
			value = gen();		
#ifdef SHOW_VALUES
				cout << boost::format("Random variate %|1$3|: %|2$20.10|.\n") % i % value; 
#endif
		}											
												
		uint64_t end = timer();						
												
		uint64_t result = end - start;	

		show_timing_results(
			result, iterations, 
			qfcl::names::name_or_typename(gen), qfcl::names::name_or_typename(Distribution::method()), 
			cpu_frequency);
	}
#endif
#ifdef QUANTLIB_MT19937_UNIFORM_RNG
	{
		//typedef qfcl::random::uniform_0in_1in<> Distribution;

		//typedef qfcl::random::variate_generator<Engine, Distribution> Generator;
		//Engine e;
		//Distribution d;
		//Generator gen(e, d);

		typedef QuantLib::MersenneTwisterUniformRng	Generator;

		Generator gen;
	
		//volatile Generator::result_type value;
		volatile Generator::sample_type::value_type value;
												
		timer_t timer;								
												
		uint64_t start = timer();					
												
		for (CounterType i = 0; i < iterations; ++i)		
		{											
			value = gen.nextReal();		
#ifdef SHOW_VALUES
				cout << boost::format("Random variate %|1$3|: %|2$20.10|.\n") % i % value; 
#endif
		}											
												
		uint64_t end = timer();						
												
		uint64_t result = end - start;	

		show_timing_results(
			result, iterations, 
			qfcl::names::name_or_typename(gen), "quantile", 
			cpu_frequency);
	}
#endif
#ifdef BOOST_NORMAL_BOX_MULLER
	{
		typedef qfcl::random::boost_normal_distribution<> Distribution;

		typedef qfcl::random::variate_generator<Engine, Distribution> Generator;
		Engine e;
		Distribution d;
		Generator gen(e, d);
	
		volatile Generator::result_type value;
												
		timer_t timer;								
												
		uint64_t start = timer();					
												
		for (CounterType i = 0; i < iterations; ++i)		
		{											
			value = gen();		
#ifdef SHOW_VALUES
				cout << boost::format("Random variate %|1$3|: %|2$20.10|.\n") % i % value; 
#endif
		}											
												
		uint64_t end = timer();						
												
		uint64_t result = end - start;	

		show_timing_results(
			result, iterations, 
			qfcl::names::name_or_typename(gen), qfcl::names::name_or_typename(Distribution::method()), 
			cpu_frequency);
	}
#endif
#ifdef NORMAL_BOX_MULLER_POLAR
	{
		typedef qfcl::random::normal_box_muller_polar<> Distribution;

		typedef qfcl::random::variate_generator<Engine, Distribution> Generator;
		Engine e;
		Distribution d;
		Generator gen(e, d);
	
		volatile Generator::result_type value;
												
		timer_t timer;								
												
		uint64_t start = timer();					
												
		for (CounterType i = 0; i < iterations; ++i)		
		{											
			value = gen();		
#ifdef SHOW_VALUES
				cout << boost::format("Random variate %|1$3|: %|2$20.10|.\n") % i % value; 
#endif
		}											
												
		uint64_t end = timer();						
												
		uint64_t result = end - start;	

		show_timing_results(
			result, iterations, 
			qfcl::names::name_or_typename(gen), qfcl::names::name_or_typename(Distribution::method()), 
			cpu_frequency);
	}
#endif
#ifdef BOOST_NORMAL_BOX_MULLER_POLAR
	{
		typedef qfcl::random::normal_box_muller_polar<> Distribution;

		typedef boost::variate_generator<Engine, Distribution> Generator;
		Engine e;
		Distribution d;
		Generator gen(e, d);
	
		volatile Generator::result_type value;
												
		timer_t timer;								
												
		uint64_t start = timer();					
												
		for (CounterType i = 0; i < iterations; ++i)		
		{											
			value = gen();		
#ifdef SHOW_VALUES
				cout << boost::format("Random variate %|1$3|: %|2$20.10|.\n") % i % value; 
#endif
		}											
												
		uint64_t end = timer();						
												
		uint64_t result = end - start;	

		show_timing_results(
			result, iterations, 
			qfcl::names::name_or_typename(gen), qfcl::names::name_or_typename(Distribution::method()), 
			cpu_frequency);
	}
#endif
#ifdef QUANTLIB_NORMAL_BOX_MULLER_POLAR
	{
		typedef qfcl::random::QuantLib_normal_box_muller_polar<> Distribution;

		typedef qfcl::random::variate_generator<Engine, Distribution> Generator;
		Engine e;
		Distribution d;
		Generator gen(e, d);
	
		volatile Generator::result_type value;
												
		timer_t timer;								
												
		uint64_t start = timer();					
												
		for (CounterType i = 0; i < iterations; ++i)		
		{											
			value = gen();		
#ifdef SHOW_VALUES
				cout << boost::format("Random variate %|1$3|: %|2$20.10|.\n") % i % value; 
#endif
		}											
												
		uint64_t end = timer();						
												
		uint64_t result = end - start;	

		show_timing_results(
			result, iterations, 
			qfcl::names::name_or_typename(gen), qfcl::names::name(Generator::method()), 
			cpu_frequency);
	}
#endif
#ifdef QUANTLIB_NATIVE_NORMAL_BOX_MULLER_POLAR
	{
		typedef QuantLib::MersenneTwisterUniformRng	UniformGenerator;
		UniformGenerator uniformGen;

		typedef QuantLib::BoxMullerGaussianRng<UniformGenerator> Generator;

		Generator gen(uniformGen);

		//typedef qfcl::random::standard::QuantLib_normal_box_muller_polar<> Distribution;

		//typedef qfcl::random::standard::variate_generator<Engine, Distribution> Generator;
		//Engine e;
		//Distribution d;
		//Generator gen(e, d);
	
		volatile Generator::sample_type::value_type value;
												
		timer_t timer;								
												
		uint64_t start = timer();					
												
		for (CounterType i = 0; i < iterations; ++i)		
		{											
			value = gen.next().value;		
#ifdef SHOW_VALUES
				cout << boost::format("Random variate %|1$3|: %|2$20.10|.\n") % i % value; 
#endif
		}											
												
		uint64_t end = timer();						
												
		uint64_t result = end - start;	

		show_timing_results(
			result, iterations, 
			qfcl::names::name_or_typename(gen), string(), //qfcl::names::name_or_typename(Distribution::method()), 
			cpu_frequency);
	}
#endif
#ifdef QUANTLIB_WRAPPED_NATIVE_NORMAL_BOX_MULLER_POLAR
	{
		typedef qfcl::random::QuantLib_BoxMullerGaussianRng<> Generator;

		Generator gen;
	
		volatile Generator::result_type value;
												
		timer_t timer;								
												
		uint64_t start = timer();					
												
		for (CounterType i = 0; i < iterations; ++i)		
		{											
			value = gen();		
#ifdef SHOW_VALUES
				cout << boost::format("Random variate %|1$3|: %|2$20.10|.\n") % i % value; 
#endif
		}											
												
		uint64_t end = timer();						
												
		uint64_t result = end - start;	

		show_timing_results(
			result, iterations, 
			qfcl::names::name_or_typename(gen), qfcl::names::name(Generator::method()),
			cpu_frequency);
	}
#endif	
#ifdef BOOST_NORMAL_ZIGGURAT
	{
		typedef qfcl::random::boost_normal_ziggurat<> Distribution;

		typedef boost::random::variate_generator<Engine, Distribution> Generator;
		Engine e;
		Distribution d;
		Generator gen(e, d);
	
		volatile Distribution::result_type value;
												
		timer_t timer;								
												
		uint64_t start = timer();					
												
		for (CounterType i = 0; i < iterations; ++i)		
		{											
			value = gen();		
#ifdef SHOW_VALUES
				cout << boost::format("Random variate %|1$3|: %|2$20.10|.\n") % i % value; 
#endif
		}											
												
		uint64_t end = timer();						
												
		uint64_t result = end - start;	

		show_timing_results(
			result, iterations, 
			qfcl::names::name_or_typename(gen), "Ziggurat",
			cpu_frequency);
	}
#endif	
#ifdef QUANTLIB_NATIVE_NORMAL_ZIGGURAT
	{
		typedef QuantLib::ZigguratRng Generator;

		// Use default MT seed.
		Generator gen(5489);
	
		volatile Generator::sample_type::value_type value;
												
		timer_t timer;								
												
		uint64_t start = timer();					
												
		for (CounterType i = 0; i < iterations; ++i)		
		{											
			value = gen.next().value;		
#ifdef SHOW_VALUES
				cout << boost::format("Random variate %|1$3|: %|2$20.10|.\n") % i % value; 
#endif
		}											
												
		uint64_t end = timer();						
												
		uint64_t result = end - start;	

		show_timing_results(
			result, iterations, 
			qfcl::names::name_or_typename(gen), "Ziggurat",
			cpu_frequency);
	}
#endif	
#ifdef QUANTLIB_NORMAL_ZIGGURAT
	{
		typedef qfcl::random::QuantLib_ZigguratRng Generator;

		// Use default MT seed.
		Generator gen;
	
		volatile Generator::result_type value;
												
		timer_t timer;								
												
		uint64_t start = timer();					
												
		for (CounterType i = 0; i < iterations; ++i)		
		{											
			value = gen();		
#ifdef SHOW_VALUES
				cout << boost::format("Random variate %|1$3|: %|2$20.10|.\n") % i % value; 
#endif
		}											
												
		uint64_t end = timer();						
												
		uint64_t result = end - start;	

		show_timing_results(
			result, iterations, 
			qfcl::names::name_or_typename(gen), qfcl::names::name(Generator::method()),
			cpu_frequency);
	}
#endif	
	cout << "Press Enter to exit.";

	char c;
	cin.get(c);
}
