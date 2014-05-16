/* test/univariate_generator_speed.cpp
 *
 * Copyright (C) 2014 James Hirschorn <James.Hirschorn@gmail.com>
 *
 * Use, modification and distribution are subject to 
 * the BOOST Software License, Version 1.0. 
 * (See accompanying file LICENSE.txt)
 */

/*! \file test/univariate_generator_speed.cpp
	\brief univariate generator speed test

	\author James Hirschorn
	\date February 7, 2014
*/

#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include <boost/chrono.hpp>
#include <boost/cstdint.hpp>
#include <boost/foreach.hpp>
#include <boost/format.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/mpl/push_front.hpp>
namespace mpl = boost::mpl;
#include <boost/program_options.hpp>
namespace po = boost::program_options;
#include <boost/random.hpp>
#include <boost/timer/timer.hpp>
#include <boost/tokenizer.hpp>

#include <qfcl/defines.hpp>
#include <qfcl/random/distribution/uniform_0in_1ex.hpp>
#include <qfcl/random/distribution/normal_box_muller_polar.hpp>
#include <qfcl/random/engine/mersenne_twister.hpp>
#include <qfcl/random/generator/variate_generator.hpp>
#include <qfcl/utility/comma_separated_number.hpp>
#include <qfcl/utility/names.hpp>
#include <qfcl/utility/tmp.hpp>
#include <qfcl/utility/type_selection.hpp>

#include "utility/cpu_timer.hpp"

using namespace std;

// PRNG engine
typedef qfcl::random::mt19937 Engine;

//! list of continuous uniform generators
typedef qfcl::named_adapter< 
		mpl::vector<
				qfcl::random::variate_generator<Engine, qfcl::random::uniform_0in_1ex<>>
			>
	,	qfcl::tmp::concatenate<
				qfcl::string::prefix<qfcl::string::uniform_string, '_'>::type
			,	qfcl::string::continuous_string
			>
	>
uniform_continuous_generators;

//! list of normal univariate generators
typedef qfcl::named_adapter<
		mpl::vector<
				qfcl::random::variate_generator<Engine, qfcl::random::normal_box_muller_polar<>>
#ifdef USE_QUANTLIB
//			,	qfcl::random::QuantLib_normal_box_muller_polar<>
#endif USE_QUANTLIB
			>
	,	qfcl::string::normal_string
	>
normal_generators;

//! list of all families of generators
typedef mpl::vector<uniform_continuous_generators, normal_generators> generator_families;

//! List of all generators
// NOTE: This should be the union of generator_families.
typedef qfcl::tmp::concatenate<
		uniform_continuous_generators
	,	normal_generators
	>::type
generator_list;

// default timer
#define QFCL_TIMER rdtsc_variate_timer
// default number of iterations per engine
#define	QFCL_ITERATIONS 100000000
// output formatting
const size_t indent_width = 5;
const size_t precision = 12;

// If you want to include speed tests for variate generators not satisfying the Named concept
// (e.g. the boost::random engines)
//#define INCLUDE_UNNAMED

/** timers */

/* timing routines */

#ifdef QFCL_RDTSCP
typedef qfcl::timer::rdtscp_timer timer_type;
#else
typedef qfcl::timer::rdtsc_timer timer_type;
#endif

// output results

template<typename CounterType>
void show_timing_results(uint64_t clock_cycles, CounterType iterations, const string & generator_name, const string & variate_method, double cpu_freq)
{
	// time taken in seconds
	double time_taken = clock_cycles / cpu_freq;

	std::cout 
		<< boost::format("%1%.\nMethod: %2%\n%|3$18.2f| random numbers/second = %|4$13.8f| nanoseconds/random number = %|5$6.1f| CPU cycles/random number\n")
			% generator_name % variate_method
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
	timer_object(CounterType iter, int num_variates_disp, double freq) 
		: iterations(iter), num_variates_displayed(num_variates_disp), 
		  cpu_frequency(freq) {} 

	template<typename Generator>
	void operator()(Generator & gen) 
	{
		Timer t;
		auto result = t(gen, iterations);

//#ifdef INCLUDE_UNNAMED
		show_timing_results(result, iterations, qfcl::names::name_or_typename(gen), string(), cpu_frequency);
//#else
//		show_timing_results(result, iterations_, qfcl::names::name(dist), dist.method.name(), cpu_frequency);
//#endif // INCLUDE_UNNAMED
		
		// diagnostics: show the next num_variate_displayed variates
		const string out_line = "Random number %1%: %|2$." 
			+ boost::lexical_cast<string>(precision) + "f|\n";
		// use copies to preserve state
		Generator g = gen;
		for (int i = 0; i < num_variates_displayed; ++i)
			std::cout 
				<< boost::format(out_line) % (iterations + i + 1) % g();

		std::cout << std::endl;
	}

	CounterType iterations;
	int num_variates_displayed;
	double cpu_frequency;
};

// time stamp counter variate generation timer
struct rdtsc_variate_timer
{
	typedef uint64_t result_type;

	template<typename Generator, typename CounterType>
	result_type operator()(Generator & gen_, CounterType iterations) const
	{
		// don't want to slow down by using a reference
		Generator gen = gen_;
	
		volatile typename Generator::result_type value;
												
		timer_type timer;								
												
		uint64_t start = timer();					
												
		for (CounterType i = 0; i < iterations; ++i)		
		{											
			value = gen();							
		}											
												
		uint64_t end = timer();						
												
		uint64_t result = end - start;	

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
template<typename GeneratorList, typename CounterType, typename SelectionMethod>
void perform_speed_test(
	const vector<string> & generators_selected, 
	const string & timer_name, CounterType iterations, int num_variates_disp,
	double cpu_freq)
{
	if (timer_name == mpl::c_str<rdtsc_variate_timer::name>::value)
	{
        qfcl::type_selection::for_each_selector<GeneratorList, SelectionMethod>(
			generators_selected,
			timer_object<CounterType, rdtsc_variate_timer>(iterations, num_variates_disp, cpu_freq)
		);
	}
	//else if (timer_name == "boost")
 //       qfcl::type_selection::for_each_selector<DistributionList, SelectionMethod>(
	//		generators_selected, 
	//		timer_object<CounterType, boost_timer>(e, iterations, cpu_freq)
	//	);
	else
        throw std::logic_error("bad program");
}

// Tests a family of generators.
struct test_family_object
{
	test_family_object(vector<string>& dist_names)
		: distribution_names(dist_names)
	{}

	template<typename Family>
	void operator()(Family const& f)
	{
		distribution_names = qfcl::names::get_name_or_typenames<Family>(); 
	}

	vector<string>& distribution_names;
};

/** output function object */

// boost::lambda did not work for this.
struct named_functor_object
{
	typedef void result_type;

	named_functor_object(std::ostringstream & oss, size_t indent_width)
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
	using namespace qfcl::type_selection;

	//typedef unsigned long long CounterType;
	typedef unsigned long CounterType;

	CounterType iterations;

	// Declare the supported options.
	po::options_description generic_options("Generic options");
	generic_options.add_options()
		("help,h", "this help message")
		("version,v", "version info");
	
	string generators;
	string family;
	string timer_param;

	int num_displayed_variates;

	po::options_description generator_options("Generator options");
	generator_options.add_options()
		("generators,g", 
		 po::value<string>(&generators),
		 "specifies a single or comma separated list of generator(s) to be tested. " \
		 "All generators are tested if neither this option " \
		 "nor the -f option is specified.\n" \
		 "-g l [ --generators list ] for a list of all available generators.")
		("family,f",
		 po::value<string>(&family),
		 "specifies a family of generators to test.\n" \
		 "-f l [ --family list ] for a list of all available families.");

	po::options_description timer_options("Timer options");
	timer_options.add_options()
		("timer,t", po::value<string>(&timer_param) -> default_value(mpl::c_str<QFCL_TIMER::name>::value),
		 "specifies which timer to use. " \
		 "Type -t h [ --timer help ] for a list of all timers.");

	po::options_description diagnostic_options("Diagnostic options");
	diagnostic_options.add_options()
		("verify,r", po::value<int>(&num_displayed_variates) -> default_value(0),
		"how many variates to display.");

	po::options_description primary_options("Alternatives to positional command line parameters");
	primary_options.add_options()
		("iterations,i", po::value<CounterType>(&iterations) -> default_value(QFCL_ITERATIONS),
		 "number of random numbers to generate for each generator");

	po::options_description command_line_options;
	command_line_options.add(generic_options).add(generator_options).add(timer_options).add(diagnostic_options).add(primary_options);

	po::positional_options_description pd;
	pd.add("iterations", 1);

	po::variables_map vm;
	po::store(po::command_line_parser(argc, argv).options(command_line_options).positional(pd).run(), vm);
	po::notify(vm);    

	if (vm.count("version"))
	{
		cout << argv[0] << ", QFCL Version " << QFCL_VERSION << endl;
		cout << "Copyright 2014, James Hirschorn <James.Hirschorn@gmail.com>" << endl;
		return EXIT_SUCCESS;
	}

	// make sure that user is aware that there are options
	if (argc == 1 || vm.count("help")) 
	{
		cout << argv[0] << " measures the speed of pseudo-random number generation\n" \
			 << "of various univariate generators." << endl << endl;
		cout << "Usage: " << argv[0] << " [options] [iterations]" << endl
			 << "   OR  " << argv[0] << " [options]" << endl << endl;
		cout << "Examples: " << argv[0] << " -f normal -i 10000000" << endl;
		cout << "          " << argv[0] 
			 << " -d normal_Box-Muller{double}{uniform_0in_1ex{double}},boost-normal_Box-Muller{double} 1000000000" 
			 << endl << endl;
		cout << generic_options << endl;
		cout << generator_options << endl;
		cout << timer_options << endl;
		cout << diagnostic_options << endl;
		cout << primary_options << endl;
		if (vm.count("help"))
			return EXIT_SUCCESS;
	}

	vector<string> generators_selected; 
	//typedef all_generators generator_list;
	auto generator_names = qfcl::names::get_name_or_typenames<generator_list>(); 
	auto family_names = qfcl::names::get_name_or_typenames<generator_families>();
		
	if (vm.count("generators"))
	{
		string generators_param = vm["generators"].as<string>();

		// parse the comma separated list
		typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
		boost::char_separator<char> sep(",");
		tokenizer tk(generators_param, sep);
		for (string dist : tk)
			generators_selected.push_back(dist);

		// list of engines
		if (generators_selected.size() == 1 && (generators_selected[0] == "list" || generators_selected[0] == "l"))
		{
			std::ostringstream oss;

			BOOST_FOREACH(string s, generator_names)
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

		qfcl::type_selection::for_each_selector<generator_families>(
			family,
			test_family_object(generators_selected));
	}
	else // all engines
	{
		generators_selected = generator_names;
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

			mpl::for_each<timer_list>( named_functor_object(oss, indent_width) );

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
	perform_speed_test<generator_list, CounterType, NAME_OR_TYPENAME>(generators_selected, timer_param, iterations, 
		num_displayed_variates, cpu_frequency);
#else
	perform_speed_test<generator_list, CounterType, NAME>(generators_selected, timer_param, iterations, 
		num_displayed_variates, cpu_frequency);
#endif // INCLUDE_UNNAMED

	cout << "Press Enter to exit.";

	char c;
	cin.get(c);
}