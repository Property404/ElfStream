#include "scrub.h"
#include "FileUtil.h"
#include <elf_parser.hpp>
#include <set>
#include <string>
#include <iostream>

using namespace std::string_literals;

static constexpr char INTERRUPT_INSTRUCTION = 0xCC;
static constexpr auto MIN_ELF_SIZE = 0x400;
static constexpr auto MAX_PATH_SIZE = 0x2000;

std::string scrubElf(const std::string& elf_path, std::vector<Range>& ranges)
{
	// Guard args
	if(elf_path.size() > MAX_PATH_SIZE)
		throw std::invalid_argument(__func__+": elf_path is suspiciously large. Did you pass file contents?"s);
	if(ranges.size())
		throw std::invalid_argument(__func__+": function expects a reference to an empty Range vector"s);

	// Which sections we're interested in
	const std::set<std::string> sections_to_wipe = {".text", ".rodata"};

	// Convert sections to ranges
	elf_parser::Elf_parser elf_parser(elf_path);
	const auto sections = elf_parser.get_sections();
	for(const auto& section : sections)
	{
		const auto offset = section.section_offset;
		const auto size = section.section_size;
		if(sections_to_wipe.contains(section.section_name))
			ranges.emplace_back(offset, size);
	}

	// Create new file contents with the specified ranges removed
	const std::string original = FileUtil::getFileContents(elf_path);
	std::string shrunk;
	size_t index = 0;
	for(const auto& range:ranges)
	{
		shrunk += original.substr(index, range.start-index);
		index = range.getEnd();
	}
	shrunk += original.substr(index);

	return shrunk;
}

std::string expandScrubbedElf(const std::string& contents, const std::vector<Range>& ranges)
{
	// Guard args
	if(contents.size() < MIN_ELF_SIZE)
		throw std::invalid_argument(__func__+": contents is suspiciously small. Did you pass in a path?"s);

	// Create new expanded contents
	std::string expanded;
	size_t index = 0;
	size_t accu = 0;
	for(const auto& range:ranges)
	{
		expanded += contents.substr(index, range.start-accu-index);
		expanded += std::string(range.size, INTERRUPT_INSTRUCTION);
		index = range.start - accu;
		accu += range.size;
	}
	expanded += contents.substr(index);

	return expanded;
}
