#include <functional>
#include <string>
using std::string;

#include <boost/format.hpp>
#include <boost/mpl/for_each.hpp>
#include <boost/mpl/vector.hpp>
namespace mpl = boost::mpl;
#include <boost/random/mersenne_twister.hpp>
#include <boost/timer/timer.hpp>

#include <qfcl/random/engine/boost_mt19937.hpp>
#include <qfcl/random/engine/mersenne_twister.hpp>
#include <qfcl/utility/comma_separated_number.hpp>
#include <qfcl/utility/names.hpp>
#include <qfcl/utility/type_traits.hpp>

#include "utility/cpu_timer.hpp"

// which TSC to use
typedef qfcl::timer::rdtsc_timer timer_t;

template<typename CounterType>
void show_timing_results(uint64_t clock_cycles, CounterType iterations, const string & engine_name, double cpu_freq)
{
	// time taken in seconds
	double time_taken = clock_cycles / cpu_freq;

	std::cout 
		<< boost::format("%|1$25|: %|2$13.2f| random numbers/second = %|3$11.8f| nanoseconds/random number = %|4$5.1f| CPU cycles/random number\n\n")
			% engine_name % ( iterations / time_taken ) 
			% ( time_taken * UINT64_C(1000000000) / iterations ) 
			% ( double(clock_cycles) / iterations);
}

int main(int argc, char * argv[])
{
	using namespace std;
	using namespace boost::timer;
	using namespace qfcl::random;

	typedef unsigned long long CounterType;
	//typedef unsigned long CounterType;

	CounterType iterations = 1000000000;
	// For debugging purposes only
	size_t dump_size = 0; //10;

#ifndef	SYSTEM_CPU_FREQUENCY
	double cpu_frequency = qfcl::timer::get_cpu_frequency();
#else
	double cpu_frequency = SYSTEM_CPU_FREQUENCY;
#endif	// SYSTEM_CPU_FREQUENCY

	cout << boost::format("CPU frequency: %|1$.5| GHz\n\n") % (cpu_frequency / qfcl::timer::one_second);

	cout << qfcl::io::custom_formatted(iterations) << " iterations per engine:" << endl << endl;

	//cout << "engine name: " << qfcl::tmp::name_or_typename<boost::random::mt19937>::name() << endl;

	/*timer_functor<CounterType> timer(iterations, cpu_frequency);
	boost_mt19937 eng1;
	mt19937 eng2;
	timer(eng1);
	timer(eng2);*/
	
	// MT19937
	{
		typedef qfcl::random::mt19937 Engine;

		Engine e;
	
		volatile Engine::result_type value;
												
		timer_t timer;								
												
		uint64_t start = timer();					
												
		for (CounterType i = 0; i < iterations; ++i)		
		{											
			value = e();							
		}											
												
		uint64_t end = timer();						
												
		uint64_t result = end - start;
	
		show_timing_results(result, iterations, qfcl::names::name_or_typename(e), cpu_frequency);
	}
	cout << "Press Enter to exit.";

	char c;
	cin.get(c);
}
