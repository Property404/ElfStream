#pragma once
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

};

