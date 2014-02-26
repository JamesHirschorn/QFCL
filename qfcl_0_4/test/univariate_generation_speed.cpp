/* test/normal_univariate_speed.cpp
 *
 * Copyright (C) 2014 James Hirschorn <James.Hirschorn@gmail.com>
 *
 * Use, modification and distribution are subject to 
 * the BOOST Software License, Version 1.0. 
 * (See accompanying file LICENSE.txt)
 */

/*! \file test/normal_univariate_speed.cpp
	\brief univariate generation speed test

	\author James Hirschorn
	\date February 7, 2014
*/

#include <iostream>
#include <stdexcept>
#include <string>
using std::string;
#include <vector>
using std::vector;

#include <boost/chrono.hpp>
#include <boost/cstdint.hpp>
#include <boost/foreach.hpp>
#include <boost/format.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/mpl/push_front.hpp>
namespace mpl = boost::mpl;
#include <boost/program_options.hpp>
namespace po = boost::program_options;
#include <boost/random.hpp>
#include <boost/timer/timer.hpp>
#include <boost/tokenizer.hpp>

#include <qfcl/defines.hpp>
#include <qfcl/random/engine/mersenne_twister.hpp>
#include <qfcl/utility/comma_separated_number.hpp>
#include <qfcl/utility/names.hpp>
#include <qfcl/utility/tmp.hpp>
#include <qfcl/utility/type_selection.hpp>

#include "utility/cpu_timer.hpp"

#include "distribution_common.ipp"
using namespace ::std;

// PRNG engine
typedef qfcl::random::mt19937 Engine;
// default timer
#define QFCL_TIMER rdtsc_variate_timer
// default number of iterations per engine
#define	QFCL_ITERATIONS 100000000
// output formatting
const size_t indent_width = 5;

// If you want to include speed tests for engines not satisfying the Named concept
// (e.g. the boost::random engines)
#define INCLUDE_UNNAMED

/** timers */

/* timing routines */

#ifdef QFCL_RDTSCP
typedef qfcl::timer::rdtscp_timer timer_type;
#else
typedef qfcl::timer::rdtsc_timer timer_type;
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

template<typename CounterType>
void show_timing_results(boost::timer::cpu_times t, CounterType iterations, const string & engine_name, double cpu_freq)
{
	uint64_t clock_cycles = static_cast<uint64_t>( (t.user + t.system) * cpu_freq / UINT64_C(1000000000) );

	show_timing_results(clock_cycles, iterations, engine_name, cpu_freq);
}

/* timer object */

// generic
template<typename CounterType, typename Timer>
struct timer_object
{
	timer_object(Engine & e, CounterType iter, double freq) 
		: engine(e), iterations(iter), cpu_frequency(freq) {} 

	template<typename Distribution>
	void operator()(Distribution & dist) 
	{
		Timer t;
		auto result = t(dist, engine, iterations);

#ifdef INCLUDE_UNNAMED
		show_timing_results(result, iterations, qfcl::names::name_or_typename(dist), dist.method.name(), cpu_frequency);
#else
		show_timing_results(result, iterations_, qfcl::names::name(dist), cpu_freq_);
#endif // INCLUDE_UNNAMED
	}

	Engine & engine;
	CounterType iterations;
	double cpu_frequency;
};

// time stamp counter variate generation timer
struct rdtsc_variate_timer
{
	typedef uint64_t result_type;

	template<typename Distribution, typename CounterType>
	result_type operator()(Distribution & d_, Engine & e_, CounterType iterations) const
	{
		// don't want to slow down by using a reference
		Engine e = e_;
		Distribution d = d_;
	
		volatile typename Distribution::result_type value;
												
		timer_type timer;								
												
		uint64_t start = timer();					
												
		for (CounterType i = 0; i < iterations; ++i)		
		{											
			value = d_(e);							
		}											
												
		uint64_t end = timer();						
												
		uint64_t result = end - start;	

		// update the referenced objects
		e_ = e;
		// Distributions need not by assignment copyable.
		//d_ = d;

		return result;
	}

	static string description()
	{
		return "time stamp counter for variate generators (non-serialized)";
	}

	typedef qfcl::timer::detail::RDTSC_name name;
};


// list of timers
typedef mpl::vector<rdtsc_variate_timer> timer_list;

// We want to avoid double type-selection for now, so we use the following "kludge".
template<typename DistributionList, typename CounterType, typename SelectionMethod>
void perform_speed_test(
	const vector<string> & distributions_selected, 
	const string & timer_name, Engine & e, CounterType iterations, double cpu_freq)
{
	if (timer_name == mpl::c_str<rdtsc_variate_timer::name>::value)
        qfcl::type_selection::for_each_selector<DistributionList, SelectionMethod>(
			distributions_selected,
			timer_object<CounterType, rdtsc_variate_timer>(e, iterations, cpu_freq)
		);
	//else if (timer_name == "boost")
 //       qfcl::type_selection::for_each_selector<DistributionList, SelectionMethod>(
	//		distributions_selected, 
	//		timer_object<CounterType, boost_timer>(e, iterations, cpu_freq)
	//	);
	else
        throw std::logic_error("bad program");
}

/** output function object */

// boost::lambda did not work for this.
struct named_functor
{
	typedef void result_type;

	named_functor(std::ostringstream & oss, size_t indent_width)
		: oss_(oss), indent_width_(indent_width)
	{};

	template<typename T>
	result_type operator()(T & t)
	{
		oss_ << std::setw(indent_width_) << "" << qfcl::names::name(t) << ": " 
			<< T::description() << std::endl;
	}
	
private:
	std::ostringstream & oss_;
	size_t indent_width_;
};

int main(int argc, char * argv[])
{
	//using namespace std;
	//using namespace qfcl::random;
	using namespace qfcl::type_selection;

	typedef unsigned long long CounterType;
	//typedef unsigned long CounterType;

	CounterType iterations;

	// Declare the supported options.
	po::options_description generic_options("Generic options");
	generic_options.add_options()
		("help,h", "this help message")
		("version,v", "version info");
	
	string distributions;
	string family;
	string timer_param;

	po::options_description engine_option("Distribution and Timer options");
	engine_option.add_options()
		("distributions,d", 
		 po::value<string>(&distributions),
		 "specifies a single or comma separated list of distribution(s) to be tested. " \
		 "All distributions are tested if neither this option " \
		 "nor the -f option is specified.\n" \
		 "-d l [ --distributions list ] for a list of all available distributions.")
		("family,f",
		 po::value<string>(&family),
		 "specifies a family of distributions to test.\n" \
		 "-f l [ --family list ] for a list of all available families.")
		("timer,t", po::value<string>(&timer_param) -> default_value(mpl::c_str<QFCL_TIMER::name>::value),
		 "specifies which timer to use. " \
		 "Type -t h [ --timer help ] for a list of all timers.");

	po::options_description primary_options("Alternatives to positional command line parameters");
	primary_options.add_options()
		("iterations,i", po::value<CounterType>(&iterations) -> default_value(QFCL_ITERATIONS),
		 "number of random numbers to generate for each distribution");

	po::options_description command_line_options;
	command_line_options.add(generic_options).add(engine_option).add(primary_options);

	po::positional_options_description pd;
	pd.add("iterations", 1);

	po::variables_map vm;
	po::store(po::command_line_parser(argc, argv).options(command_line_options).positional(pd).run(), vm);
	po::notify(vm);    

	if (vm.count("version"))
	{
		cout << argv[0] << ", QFCL Version " << QFCL_VERSION << endl;
		cout << "Copyright 2012, James Hirschorn <James.Hirschorn@gmail.com>" << endl;
		return EXIT_SUCCESS;
	}

	// make sure that user is aware that there are options
	if (argc == 1 || vm.count("help")) 
	{
		cout << argv[0] << " measures the speed of pseudo-random number generation\n" \
			 << "of various engines." << endl << endl;
		cout << "Usage: " << argv[0] << " [options] [iterations]" << endl
			 << "   OR  " << argv[0] << " [options]" << endl << endl;
		cout << "Example: " << argv[0] << " -e MT19937 -e boost-MT19937 10000000" << endl << endl;
		cout << generic_options << endl;
		cout << engine_option << endl;
		cout << primary_options << endl;
		if (vm.count("help"))
			return EXIT_SUCCESS;
	}

	vector<string> distributions_selected; 
#ifdef INCLUDE_UNNAMED
//	typedef mpl::push_front<all_distributions, boost::random::uniform_01<> >::type distribution_list;
#else
	typedef all_distributions distribution_list;
#endif // INCLUDE_UNNAMED
	typedef all_distributions distribution_list;

	auto distribution_names = qfcl::names::get_name_or_typenames<distribution_list>(); 
	auto family_names = qfcl::names::get_name_or_typenames<distribution_families>();
		
	if (vm.count("distributions"))
	{
		string distributions_param = vm["distributions"].as<string>();

		// parse the comma separated list
		typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
		boost::char_separator<char> sep(",");
		tokenizer tk(distributions_param, sep);
		for (string dist : tk)
			distributions_selected.push_back(dist);

		// list of engines
		if (distributions_selected.size() == 1 && (distributions_selected[0] == "list" || distributions_selected[0] == "l"))
		{
			std::ostringstream oss;

			BOOST_FOREACH(string s, distribution_names)
			{
				oss << std::setw(indent_width) << "" << s << std::endl;
			}

			cout << oss.str();

			return EXIT_SUCCESS;
		}
	}
	else if (vm.count("family"))
	{
		// list of families
		if (family == "list" || family == "l")
		{
			ostringstream oss;

			for (string s : family_names)
			{
				oss << setw(indent_width) << "" << s << endl;
			}

			cout << oss.str();

			return EXIT_SUCCESS;
		}
		if (family == "normal")
		{
			distributions_selected = qfcl::names::get_name_or_typenames<normal_distributions>(); 
		}
	}
	else // all engines
	{
		distributions_selected = distribution_names;
	}

	auto timer_names = qfcl::names::get_names<timer_list>();

	// list of timers
	if (vm.count("timer"))
	{

		if (timer_param == "help" || timer_param == "h")
		{
			using namespace boost::lambda;

			std::ostringstream oss;
			const size_t indent_width = 5;

			mpl::for_each<timer_list>( named_functor(oss, indent_width) );

			cout << oss.str();

			return EXIT_SUCCESS;
		}
		else
		{
            if (find(QFCL_NONMEMBER_BEGIN(timer_names), QFCL_NONMEMBER_END(timer_names), timer_param) == QFCL_NONMEMBER_END(timer_names))
				throw po::invalid_option_value(timer_param + " is not a valid timer.");
		}
	}
#ifndef	SYSTEM_CPU_FREQUENCY
    double cpu_frequency = qfcl::timer::get_cpu_frequency();
#else
	double cpu_frequency = SYSTEM_CPU_FREQUENCY;
#endif	// SYSTEM_CPU_FREQUENCY

    cout << boost::format("CPU frequency: %|1$.5| GHz\n\n") % (cpu_frequency / qfcl::timer::one_second);

	cout << qfcl::io::custom_formatted(iterations) << " iterations per engine:" << endl << endl;

	Engine e;
#ifdef INCLUDE_UNNAMED
	perform_speed_test<distribution_list, CounterType, NAME_OR_TYPENAME>(distributions_selected, timer_param, e, iterations, cpu_frequency);
#else
	perform_speed_test<distribution_list, CounterType, NAME>(distributions_selected, timer_param, e, iterations, cpu_frequency);
#endif // INCLUDE_UNNAMED

	cout << "Press Enter to exit.";

	char c;
	cin.get(c);
}