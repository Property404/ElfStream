#pragma once
#include "Range.h"
#include "scrub.h"
#include <string>
#include <vector>

class AbstractElfAccessor
{
		// Block size should be multiple of native page size
		static constexpr ssize_t BLOCK_SIZE = 0x1000;
	public:
		struct Patch{
			size_t start;
			size_t size;
			std::string content;

			// Convert to a string for transfer over the network
			std::string serialize()const
			{
				return std::to_string(start)+" "+std::to_string(size)+" "+content;
			}

			// Apply a patch using a custom applicator
			// in chunks of `chunk_size` bytes
			template<class Chunk>
			void apply(auto&& applicator) const
			{
				const auto chunk_size = sizeof(Chunk);
				const auto patch_end  = start + size;
				if(chunk_size < 1 || chunk_size > sizeof(uint64_t))
					throw std::invalid_argument("Invalid chunk size");

				for(unsigned i=start;i<patch_end;i+=chunk_size)
				{
					Chunk chunk = 0;
					for(unsigned j=0;j<chunk_size;j++)
					{
						Chunk new_byte = 0;
						const size_t index = i+j-start;
						if(index < size)
							new_byte = static_cast<unsigned char>(content.at(index));
						chunk += (Chunk)(new_byte) << ((Chunk)(8)*j);
					}
					applicator(i, chunk);
				}

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
		virtual std::string getExpandedElf() 
		{
			std::vector<Range> ranges;
			const auto contents = getBlankElf(ranges);
			return expandScrubbedElf(contents, ranges);
		}

		void* alignToBlockStart(const void* address) const noexcept
		{
			uintptr_t improved = (uintptr_t) address;
			improved &= ~(getBlockSize()-1);
			return (void*)improved;
		}

		uintptr_t alignToBlockStart(const uintptr_t address) const noexcept
		{
			return (uintptr_t)alignToBlockStart((void*)address);
		}

		size_t getBlockSize() const noexcept
		{
			return BLOCK_SIZE;
		}
};

