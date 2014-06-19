#include <algorithm>
#include <memory>
#include <string>

extern "C"
{
#include "gdef.h"
#include "unif01.h"
#include "bbattery.h"
}

#include <qfcl/defines.hpp>
#include <qfcl/random/engine/mersenne_twister.hpp>
#include <qfcl/random/distribution/normal_box_muller_polar.hpp>
#include <qfcl/random/generator/variate_generator.hpp>
#include <qfcl/utility/names.hpp>

typedef qfcl::random::mt19937 Engine;

Engine engine;

Engine::result_type mt19937_GenBits()
{
	return engine();
}


int main (void)
{
	unif01_Gen *gen;
	//gen = unif01_CreateExternGen01 ("MRG32k3a", MRG32k3a);
	//bbattery_SmallCrush (gen);
	//unif01_DeleteExternGen01 (gen);
	// initialize seed
	engine = Engine();
	//mt19937_GenBits(Engine::default_seed);
	//engine.seed(Engine::default_seed);
	std::string name_str(qfcl::names::name(Engine()));
	std::shared_ptr<char> name(new char [name_str.length()]); 
	std::copy(name_str.begin(), name_str.end(), name.get());
	gen = unif01_CreateExternGenBits(name.get(), mt19937_GenBits);
	bbattery_SmallCrush(gen);
	bbattery_Crush(gen);
	unif01_DeleteExternGenBits (gen);
	return 0;
}