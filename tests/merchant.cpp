#include "catch.hpp"
#include "Merchant.h"
#include "Parser.h"
#include "FileUtil.h"
#include <cstdlib>
#include <iostream>

using namespace std::string_literals;

TEST_CASE("Merchant-Parser symmetry", "[Merchant]")
{
	const std::vector<std::string> paths = {
		"samples/fizzbuzz",
		"samples/primes",
		"samples/hello",
	};

	for(const std::string& path: paths)
	{
		Parser parser(path);
		Merchant merchant("localhost", "tests/"+path);

		// Make sure blank elf arrives safely
		std::vector<Range> mranges;
		std::vector<Range> pranges;
		const std::string from_merchant = merchant.getBlankElf(mranges);
		const std::string from_parser = parser.getBlankElf(pranges);

		REQUIRE(mranges.size() == pranges.size());
		REQUIRE(from_merchant == from_parser);
		for(unsigned i=0;i<mranges.size();i++)
		{
			REQUIRE(mranges[i].first == pranges[i].first);
			REQUIRE(mranges[i].second == pranges[i].second);
		}

		// Check scalar values are same
		REQUIRE(merchant.memoryStart() == parser.memoryStart());
		REQUIRE(merchant.memorySize() == parser.memorySize());
		REQUIRE(merchant.textStart() == parser.textStart());
		REQUIRE(merchant.getBlockSize() == parser.getBlockSize());


		// Make sure patches arrive safely
		std::cout<<"test patches: "<<path<<std::endl;
		for
		(
				uintptr_t address=reinterpret_cast<uintptr_t>(parser.memoryStart());
				address < reinterpret_cast<uintptr_t>( parser.memoryStart()) +parser.memorySize()
				// Get rid of this after fixing network stuff
				-0x10000; // necessary to not block forever on recv
				address+=parser.getBlockSize()
		)
		{
			Merchant::PatchList merchant_patches;
			Parser::PatchList parser_patches;

			parser.fetchPatches(reinterpret_cast<void*>(address), parser_patches);

			merchant.fetchPatches(reinterpret_cast<void*>(address), merchant_patches);

			REQUIRE(parser_patches.size() == merchant_patches.size());
			for(unsigned int i=0;i<parser_patches.size();i++)
			{
				REQUIRE(parser_patches[i].start == merchant_patches[i].start);
				REQUIRE(parser_patches[i].size == merchant_patches[i].size);
				REQUIRE(parser_patches[i].content == merchant_patches[i].content);
				REQUIRE(parser_patches[i].serialize() == parser_patches[i].serialize());
			}
		}

	}
}
