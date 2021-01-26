#include "Merchant.h"
#include <elfio/elfio.hpp>
#include <list>
#include <vector>
#include <cassert>
#include <iostream>

static constexpr ssize_t BLOCK_SIZE = 0x1000;

// Corresponds to an ELF segment
struct Range
{
	uintptr_t vmem_start;
	size_t vmem_size;

	size_t content_size;
	std::string content;
};

// Temporary implementation while we build full structure
// Sucks information out of a local elf file
struct Merchant::Impl
{
	std::list<Range> ranges;
	void* first_address;
	void* last_address;
};


Merchant::Merchant(const std::string& elf_path):pimpl(std::make_unique<Impl>())
{
	ELFIO::elfio reader;
	reader.load(elf_path);

	for(unsigned i=0;i<reader.segments.size();i++)
	{
		const auto segment = reader.segments[i];
		if(segment->get_type() != PT_LOAD)
			continue;
		if(segment->get_virtual_address() == 0)
			continue;
		Range range;
		range.vmem_start = segment->get_virtual_address();
		range.vmem_size = segment->get_memory_size();
		range.content_size = segment->get_file_size();
		range.content = std::string(segment->get_data(), range.content_size);
		pimpl->ranges.push_back(std::move(range));

		if(!pimpl->first_address)
			pimpl->first_address = (void*)range.vmem_start;
		if(!pimpl->last_address)
			pimpl->last_address = (void*)(range.vmem_start+range.vmem_size-1);
		
		pimpl->first_address = (void*) std::min((uintptr_t)range.vmem_start, (uintptr_t)pimpl->first_address);
		pimpl->last_address =  (void*) std::max(
				range.vmem_size-1+(uintptr_t)range.vmem_start,
				(uintptr_t)pimpl->last_address
		);
	}
}

void Merchant::fetchPatches(const void* exact_address, Merchant::PatchList& patches)
{
	std::string block(BLOCK_SIZE, '\0');
	for(const char c : block)
		assert(c=='\0');

	const ptrdiff_t block_start = (uintptr_t)alignToBlockStart(exact_address);
	const ptrdiff_t block_end = block_start+BLOCK_SIZE;
	for(const auto& range : pimpl->ranges)
	{
		const ptrdiff_t seg_start = range.vmem_start;
		const ptrdiff_t seg_end = range.vmem_start+range.content_size;

		if(block_start >= seg_end)
			continue;
		if(block_end <= seg_start)
			continue;

		const ptrdiff_t block_offset = std::max((ptrdiff_t)0, seg_start - block_start);

		const ptrdiff_t copy_start = std::max((ptrdiff_t)0, block_start-seg_start);
		const ptrdiff_t copy_end = std::min(block_end-seg_start, seg_end-seg_start);
		assert(copy_end-copy_start <= BLOCK_SIZE);
		assert(block_offset < BLOCK_SIZE);
		assert(block_offset + (copy_end-copy_start) <= BLOCK_SIZE);
		assert(block_offset >= 0);
		assert(block_offset+copy_end-copy_start <= BLOCK_SIZE);

		Patch patch;
		patch.start = block_offset;
		patch.size = copy_end-copy_start;
		patch.content = "";
		for(unsigned i = copy_start; i<copy_end;i++)
			patch.content += range.content.at(i);

		patches.push_back(patch);
	}
}

void* Merchant::memoryStart()
{
	return alignToBlockStart(pimpl->first_address);
}

size_t Merchant::memorySize()
{
	const ptrdiff_t start = (ptrdiff_t)memoryStart();
	const ptrdiff_t end = (ptrdiff_t)alignToBlockStart(pimpl->last_address)+BLOCK_SIZE;
	return (size_t)(end - start);
}


void* Merchant::alignToBlockStart(const void* address) const
{
	uintptr_t improved = (uintptr_t) address;
	improved &= ~(BLOCK_SIZE-1);
	return (void*)improved;
}

size_t Merchant::getBlockSize() const noexcept
{
	return BLOCK_SIZE;
}
Merchant::~Merchant() = default;
