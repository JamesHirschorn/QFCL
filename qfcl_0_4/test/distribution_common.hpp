#include <string>

#include <boost/mpl/bool.hpp>
#include <boost/mpl/for_each.hpp>
#include <boost/mpl/string.hpp>
#include <boost/mpl/vector.hpp>
namespace mpl = boost::mpl;

#include <qfcl/miscellaneous/strings.hpp>
#include <qfcl/random/distribution/boost_normal_distribution.hpp>
#include <qfcl/random/distribution/boost_uniform_01.hpp>
#include <qfcl/random/distribution/boost_normal_quantile.hpp>
#include <qfcl/random/distribution/normal_box_muller.hpp>
#include <qfcl/random/distribution/normal_box_muller_polar.hpp>
#include <qfcl/random/distribution/normal_inversion.hpp>
#include <qfcl/random/distribution/uniform_0ex_1ex.hpp>
#include <qfcl/random/distribution/uniform_0in_1ex.hpp>
#include <qfcl/random/distribution/uniform_0in_1in.hpp>
#include <qfcl/utility/tmp.hpp>
#include <qfcl/utility/named_adapter.hpp>
#include <qfcl/utility/names.hpp>

#ifdef USE_QUANTLIB
//#include <qfcl/random/engine/quantlib_mt19937.hpp>
#include <qfcl/random/distribution/QuantLib_normal_box_muller_polar.hpp>
#endif USE_QUANTLIB
#ifdef USE_CPP11
//#include <qfcl/random/engine/std_mt19937.hpp>
#endif USE_CPP11
#include <qfcl/utility/tmp.hpp>

//! list of continuous uniform distributions
typedef qfcl::named_adapter< 
		mpl::vector<
				qfcl::random::uniform_0ex_1ex<> 
			,	qfcl::random::uniform_0in_1ex<>
			,	qfcl::random::uniform_0in_1in<>
			,	qfcl::random::boost_uniform_01<>
			>
	,	qfcl::tmp::concatenate<
				qfcl::string::prefix<qfcl::string::uniform_string, '_'>::type
			,	qfcl::string::continuous_string
			>
	>
continuous_uniform_distributions;

//! list of normal univariate distributions
typedef qfcl::named_adapter<
		mpl::vector<
				qfcl::random::normal_inversion<>
			,	qfcl::random::boost_normal_quantile<>
			,	qfcl::random::normal_box_muller<> 
			,	qfcl::random::boost_normal_distribution<>
			,	qfcl::random::normal_box_muller_polar<>
#ifdef USE_QUANTLIB
			,	qfcl::random::QuantLib_normal_box_muller_polar<>
#endif USE_QUANTLIB
			>
	,	qfcl::string::normal_string
	>
normal_distributions;

//! list of all families of distributions
typedef mpl::vector<continuous_uniform_distributions, normal_distributions> distribution_families;

//! List of all distributions
typedef qfcl::tmp::concatenate<
		continuous_uniform_distributions
	,	normal_distributions
	>::type
all_distributions;

/// NOTE: Put somewhere else?
//! print the \c Engine name concatenated with \c str
template<typename Engine>
void print_engine_name(Engine & e, const std::string & str, size_t indent_width = 0)
{
	BOOST_TEST_MESSAGE( std::setw(indent_width) << "" 
												<< qfcl::names::name_or_typename(e)
                                                << str );
}

