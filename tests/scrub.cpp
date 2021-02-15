#include "catch.hpp"
#include "scrub.h"
#include "FileUtil.h"
/*
 * Test the methods in scrub.h
 */

TEST_CASE("Scrub elf file", "[scrub]")
{
	const std::string path = "samples/fizzbuzz";
	std::vector<Range> ranges;

	// Ensure scrubbed elf is smaller than original
	const auto original_elf = FileUtil::getFileContents(path);
	const auto scrubbed_elf = scrubElf(path, ranges);
	REQUIRE(scrubbed_elf.size() < original_elf.size());

	// Make sure size is expected according to ranges
	size_t total_diff = 0;
	for(const auto&range:ranges)total_diff+=range.size;
	const size_t expected_size = original_elf.size() - total_diff;
	const size_t actual_size = scrubbed_elf.size();
	REQUIRE(actual_size == expected_size);
}

TEST_CASE("Scrub and unscrub elf file", "[scrub]")
{
	const std::string path = "samples/fizzbuzz";
	const auto original_elf = FileUtil::getFileContents(path);

	std::vector<Range> ranges_a;
	std::vector<Range> ranges_b;

	// After scrubbing and expanding a file, make sure it still has its
	// original size
	const auto scrubbed_a = scrubElf(path, ranges_a);
	const auto expanded_a = expandScrubbedElf(scrubbed_a, ranges_a);
	REQUIRE(original_elf.size() == expanded_a.size());

	// Make sure doing it again will yield the same expanded-scrubbed elf file
	const auto scrubbed_b = scrubElf(FileUtil::createTemporaryFile(expanded_a), ranges_b);
	const auto expanded_b = expandScrubbedElf(scrubbed_b, ranges_b);
	REQUIRE(original_elf.size() == expanded_b.size());
	REQUIRE(scrubbed_a == scrubbed_b);
	REQUIRE(expanded_a == expanded_b);
	REQUIRE(ranges_a.size() == ranges_b.size());
	for(unsigned i=0;i<ranges_a.size();i++)
	{
		const auto ra = ranges_a[i];
		const auto rb = ranges_b[i];
		REQUIRE(ra.start == rb.start);
		REQUIRE(ra.size == rb.size);
	}
}
