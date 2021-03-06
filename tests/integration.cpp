#include "catch.hpp"
#include "Agent.h"
#include "Merchant.h"
#include "FileUtil.h"
#include <cstdlib>
#include <ctime>
#include <string>
/* Test cases for the whole system */

using namespace std::string_literals;

TEST_CASE("Spawn inferior from server", "[integration]")
{
	const std::string base_name = GENERATE("hello", "fizzbuzz", "primes");
	const std::string test_programs[] = {base_name, base_name+".pie", base_name+".static"};

	srand(time(nullptr));

	for(const auto& program : test_programs)
	{
		const std::string output_file = "/tmp/"+program+"."+std::to_string(rand())+".actual.output"s;

		auto merchant = std::make_shared<Merchant>("localhost", "tests/samples/"+program);

		Agent agent(merchant);
		agent.redirectOutput(output_file);
		agent.spawn();
		agent.run();

		const std::string actual = FileUtil::getFileContents(output_file);
		const std::string expected = FileUtil::getFileContents("samples/"+base_name+".output");

		REQUIRE(actual == expected);
	}
}
