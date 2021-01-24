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
};


Merchant::Merchant(const std::string& elf_path):pimpl(std::make_unique<Impl>())
{
	ELFIO::elfio reader;
	reader.load(elf_path);

	for(unsigned i=0;i<reader.segments.size();i++)
	{
		const auto segment = reader.segments[i];
		Range range;
		range.vmem_start = segment->get_virtual_address();
		range.vmem_size = segment->get_memory_size();
		range.content_size = segment->get_file_size();
		range.content = std::string(segment->get_data(), range.content_size);
		pimpl->ranges.push_back(std::move(range));
	}
}

std::string Merchant::fetchBlockOf(const void* exact_address)
{
	std::string block(BLOCK_SIZE, '\0');

	const ptrdiff_t block_start = (uintptr_t)alignToBlockStart(exact_address);
	const ptrdiff_t block_end = block_start+BLOCK_SIZE;
	for(const auto& range : pimpl->ranges)
	{
		const ptrdiff_t seg_start = range.vmem_start;
		const ptrdiff_t seg_end = range.vmem_start+range.content_size;
		std::cout<<"..."<<std::endl;

		if(block_start >= seg_end)
			continue;
		if(block_end <= seg_start)
			continue;

		const ptrdiff_t block_offset = std::max((ptrdiff_t)0, seg_start - block_start);
		const ptrdiff_t copy_start = std::max((ptrdiff_t)0, block_start-seg_start);
		const ptrdiff_t copy_end = std::min(block_end-seg_start, seg_end-seg_start);
		std::cout<<"Block offset: "<<block_offset<<std::endl;
		assert(copy_end-copy_start <= BLOCK_SIZE);
		assert(block_offset < BLOCK_SIZE);
		assert(block_offset + (copy_end-copy_start) <= BLOCK_SIZE);
		assert(block_offset >= 0);
		assert(block_offset+copy_start <= BLOCK_SIZE);
		std::cout<<std::hex<<"Copying "<<copy_start+seg_start<<" to "<<copy_end + seg_start<<std::endl;
		for(unsigned i = copy_start; i<copy_end;i++)
		{
			block[block_offset + i] = range.content[i];
		}
	}

	return block;
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
