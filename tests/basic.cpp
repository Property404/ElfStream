#include "catch.hpp"
#include "Agent.h"
#include "Parser.h"
#include "FileUtil.h"
#include <cstdlib>
#include <ctime>
#include <string>

/* Test running the inferior from a local elf file
 * No networking is involved in these tests */

TEST_CASE("Local inferior spawning", "[Agent][Parser]")
{
	const struct {
		std::string file;
		std::string expected_output;
	} test_vectors[] = {
		{"hello.elf", "Hello World!\n"}
	};

	srand(time(nullptr));

	for(const auto& tv : test_vectors)
	{
		const std::string output_file = "/tmp/"+std::to_string(rand())+".output";

		auto parser = std::make_shared<Parser>("tests/samples/"+tv.file);
		Agent agent(parser);
		agent.redirectOutput(output_file);
		agent.spawn();
		agent.run();

		REQUIRE(FileUtil::getFileContents(output_file) == tv.expected_output);
	}
}
