#include "catch.hpp"
#include "scrub.h"
#include "FileUtil.h"
/*
 * Test the methods in scrub.h
 */

TEST_CASE("Scrub elf file", "[scrub]")
{
	const std::string path = "samples/hello";
	std::vector<Range> ranges;

	// Ensure scrubbed elf is smaller than original
	const auto original_elf = FileUtil::getFileContents(path);
	const auto scrubbed_elf = scrubElf(path, ranges);
	REQUIRE(scrubbed_elf.size() < original_elf.size());

	// Make sure size is expected according to ranges
	size_t total_diff = 0;
	for(const auto&range:ranges)total_diff+=range.second;
	const size_t expected_size = original_elf.size() - total_diff;
	const size_t actual_size = scrubbed_elf.size();
	REQUIRE(actual_size == expected_size);
}

TEST_CASE("Scrub and unscrub elf file", "[scrub]")
{
	const std::string path = "samples/hello";
	std::vector<Range> ranges;

	// After scrubbing and expanding a file, make sure it still has its
	// original size
	const auto original_elf = FileUtil::getFileContents(path);
	const auto scrubbed_a = scrubElf(path, ranges);
	const auto expanded_a = expandScrubbedElf(scrubbed_a, ranges);
	REQUIRE(original_elf.size() == expanded_a.size());
}
