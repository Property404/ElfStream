#include "Parser.h"
#include "FileUtil.h"
#include "scrub.h"
#include "elf_parser.hpp"
#include "Range.h"
#include <vector>
#include <map>
#include <cassert>
#include <iostream>

// Temporary implementation while we build full structure
// Sucks information out of a local elf file
struct Parser::Impl
{
	void* first_address = nullptr;
	void* last_address = nullptr;
	void* entry_address = nullptr;

	std::string elf_path;

	std::map<uintptr_t, Range> translation_map;
	std::vector<Range> elf_ranges;
	std::vector<std::string> range_contents;
	std::string blank_elf_contents;
};


Parser::Parser(const std::string& elf_path):pimpl(std::make_unique<Impl>())
{
	pimpl->elf_path = elf_path;


	// Gather up address information
	const elf_parser::Elf_parser elf_parser(elf_path);
	const auto segments = elf_parser.get_segments();

	pimpl->entry_address = (void*)(elf_parser.get_entry_point());

	for(const auto& segment: segments)
	{
		const uintptr_t virtual_address = segment.segment_virtaddr;
		const size_t offset = segment.segment_offset;
		const size_t size = segment.segment_filesize;

		// Store important addresses
		if(segment.segment_type == "LOAD")
		{
			if(pimpl->first_address == nullptr)
				pimpl->first_address = reinterpret_cast<void*>(virtual_address);

			// Fill translation map
			pimpl->translation_map[virtual_address] = Range(offset, size);
		}
	}

	// Scrub elf and store information about which parts were scrubbed
	pimpl->blank_elf_contents = scrubElf(elf_path, pimpl->elf_ranges);
	pimpl->last_address = alignToBlockStart(reinterpret_cast<void*>(translateOffsetToAddress(
				pimpl->elf_ranges.back().getEnd())+getBlockSize()));

	// Get contents for each range
	const auto memory_map = FileUtil::getFileContents(elf_path);
	for(const auto& range: pimpl->elf_ranges)
	{
		std::string contents = "";
		for(size_t offset=range.start;offset<range.getEnd();offset++)
		{
			contents += memory_map[offset];
		}
		pimpl->range_contents.push_back(std::move(contents));
	}
}

void Parser::fetchPatches(const void* exact_address, Parser::PatchList& patches)
{
	const uintptr_t aligned_address = (uintptr_t)alignToBlockStart(exact_address);
	for(unsigned i=0;i<pimpl->elf_ranges.size();i++)
	{
		const auto& range = pimpl->elf_ranges[i];
		const auto& range_contents = pimpl->range_contents[i];

		const auto range_start_address = translateOffsetToAddress(range.start);
		const auto range_end_address = translateOffsetToAddress(range.getEnd()-1)+1;

		if( range_end_address <= aligned_address||
			range_start_address >= aligned_address+getBlockSize()
		)
		{
			continue;
		}


		AbstractElfAccessor::Patch patch;
		if(range_start_address > aligned_address)
			patch.start = range_start_address - aligned_address;
		else
			patch.start = 0;

		patch.size = std::min(getBlockSize()-patch.start, range_end_address-aligned_address);
		patch.size = std::min(patch.size, range.size);

		const auto contents_offset = aligned_address>range_start_address?
			aligned_address-range_start_address:0;
		for(unsigned i = 0;i<patch.size;i++)
			patch.content += range_contents[contents_offset + i];

		patches.push_back(patch);
	}
}

std::string Parser::getBlankElf(std::vector<Range>& ranges)
{
	for(const auto& range: pimpl->elf_ranges)
		ranges.push_back(range);
	return pimpl->blank_elf_contents;
}

void* Parser::entryPoint()
{
	return pimpl->entry_address;
}

void* Parser::memoryStart()
{
	return alignToBlockStart(pimpl->first_address);
}

size_t Parser::memorySize()
{
	const ptrdiff_t start = (ptrdiff_t)memoryStart();
	const ptrdiff_t end = (ptrdiff_t)alignToBlockStart(pimpl->last_address)+getBlockSize();
	return (size_t)(end - start);
}

size_t Parser::translateAddressToOffset(uintptr_t address) const
{
	for(const auto& translation : pimpl->translation_map)
	{
		const auto translation_address = translation.first;
		const auto translation_offset = translation.second.start;
		const auto translation_size = translation.second.size;
		if(translation_address > address)
			break;
		if(address >= translation_address&& address<(translation_address + translation_size))
			return translation_offset;
	}
	throw std::runtime_error("Address doesn't have an associated offset");
}
uintptr_t Parser::translateOffsetToAddress(size_t offset) const
{
	for(const auto& translation : pimpl->translation_map)
	{
		const auto translation_address = translation.first;
		const auto translation_offset = translation.second.start;
		const auto translation_size = translation.second.size;

		if(translation_offset > offset)
			break;
		if(offset >= translation_offset && offset<(translation_offset + translation_size))
			return translation_address+offset-translation_offset;
	}
	throw std::runtime_error("Offset doesn't have an associated address");
}
std::string Parser::getPath() const
{
	return pimpl->elf_path;
}
Parser::~Parser() = default;
