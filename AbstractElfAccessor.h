#pragma once
#include "Range.h"
#include <string>
#include <vector>

class AbstractElfAccessor
{
	public:
		struct Patch{
			size_t start;
			size_t size;
			std::string content;

			inline std::string serialize()const
			{
				return std::to_string(start)+" "+std::to_string(size)+" "+content;
			}
		};
		using PatchList = std::vector<Patch>;

		// Fetch patches within block containing `address`
		// `Address` may be any address inside a valid block
		// `Patches` is filled with patches for the block
		virtual void fetchPatches(const void* address, PatchList& patches) = 0;

		// Returns address of first block
		virtual void* memoryStart() = 0;

		// Returns first address of executable memory
		// (not block aligned)
		virtual void* textStart() = 0;

		// Returns total size of relevant regions
		// (Regions that merchant transfers and agent protects)
		virtual size_t memorySize() = 0;

		// Get the clipped version of elf file
		virtual std::string getBlankElf(std::vector<Range>& ranges) = 0;

		// Block size is a multiple of native page size
		virtual void* alignToBlockStart(const void* address) const = 0;
		virtual size_t getBlockSize() const noexcept = 0;
};

