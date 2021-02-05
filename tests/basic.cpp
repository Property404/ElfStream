#include "catch.hpp"
#include "Agent.h"
#include "Parser.h"
#include "FileUtil.h"
#include <cstdlib>
#include <ctime>
#include <string>

using namespace std::string_literals;

/* Test running the inferior from a local elf file
 * No networking is involved in these tests */

TEST_CASE("Local inferior spawning", "[Agent][Parser]")
{
	const std::string test_programs[] = {"hello", "fizzbuzz", "primes"};

	srand(time(nullptr));

	for(const auto& program : test_programs)
	{
		const std::string output_file = "/tmp/"+program+"."+std::to_string(rand())+".actual.output"s;

		auto parser = std::make_shared<Parser>("samples/"+program);
		Agent agent(parser);
		agent.redirectOutput(output_file);
		agent.spawn();
		agent.run();

		const std::string actual = FileUtil::getFileContents(output_file);
		const std::string expected = FileUtil::getFileContents("samples/"+program+".output");

		REQUIRE(actual == expected);
	}
}
