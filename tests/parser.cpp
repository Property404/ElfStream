#include "catch.hpp"
#include "Parser.h"
#include "FileUtil.h"
#include "Range.h"
#include "scrub.h"
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <vector>
using namespace std::string_literals;

static std::string toHex(auto&& number)
{
	std::stringstream ss;
	ss<<"0x"<<std::hex<<+number<<std::endl;
	std::string info_data;
	ss>>info_data;
	return info_data;
}

TEST_CASE("Parser sanity check", "[Parser]")
{
	const std::string base_path = "samples/"s + GENERATE("hello", "fizzbuzz","primes");
	const std::string paths[] = {base_path, base_path+".pie", base_path+".static"};
	for(const auto&path: paths)
	{
		Parser parser(path);
		REQUIRE(parser.memoryStart()<=parser.entryPoint());
		REQUIRE(parser.memorySize() > 0);
	}
}

TEST_CASE("Elf reconstruction", "[Parser]")
{
	const std::string base_path = "samples/"s + GENERATE("hello", "fizzbuzz","primes");
	const std::string paths[] = {base_path, base_path+".pie", base_path+".static"};

	for(const auto& path: paths)
	{
		const std::string original_contents = FileUtil::getFileContents(path);
		Parser parser(path);

		const uintptr_t translation = parser.translateOffsetToAddress(0x00);

		// Start with blank/expanded elf file
		std::string contents = parser.getExpandedElf();

		// Rebuild elf file using patches from parser
		const auto start = reinterpret_cast<uintptr_t>(parser.memoryStart());
		const auto size = parser.memorySize();
		for(uintptr_t address = start;address<size+start;address+=parser.getBlockSize())
		{
			AbstractElfAccessor::PatchList patches;
			parser.fetchPatches(reinterpret_cast<const void*>(address), patches); 

			for(const auto& patch : patches)
			{
				// Apply patch
				patch.apply<char>([&contents, &original_contents, address, translation]
						(unsigned offset, char word){
					const size_t index = address-translation+offset;
					INFO("Index: "+toHex(index)+"\t PrevValue:"+toHex(contents.at(index)));

					contents.at(index) = word;

					// Make sure patch is being applied correctly
					REQUIRE(toHex(contents.at(index)) == toHex(original_contents.at(index)));
				});
			}

			// Confirm all patches for this range were applied correctly
			for(unsigned i=address;i<address+parser.getBlockSize();i++)
			{
				INFO("Vaddress: "+toHex(i));
				REQUIRE(contents.at(i-translation) == original_contents.at(i-translation));
			}
		}

		// Confirm we reconstructed ELF
		REQUIRE(contents.size() == original_contents.size());
		REQUIRE(contents == original_contents);
		/*
		for(unsigned i=0;i<contents.size();i++)
		{
			INFO("Location: " + toHex(i));
			REQUIRE(contents[i] == original_contents[i]);
		}
		*/
	}
}
