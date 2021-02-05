#pragma once
#include <string>
#include <memory>
#include <list>
#include "AbstractElfAccessor.h"

class Merchant: public AbstractElfAccessor
{
	public:
		Merchant(const std::string& host, const std::string& elf_path);
		~Merchant();

		// Fetch patches within block containing `address`
		// `Address` may be any address inside a valid block
		// `Patches` is filled with patches for the block
		void fetchPatches(const void* address, PatchList& patches) override;

		// Returns address of first block
		void* memoryStart() override;

		// Returns first address of executable memory
		// (not block aligned)
		void* textStart() override;

		// Returns total size of relevant regions
		// (Regions that merchant transfers and agent protects)
		size_t memorySize() override;

		// Get the zero-ed out elf contents
		std::string getBlankElf() override;

		// Block size is a multiple of native page size
		void* alignToBlockStart(const void* address) const override;
		size_t getBlockSize() const noexcept override;
	private:
		// Filthy, dirty, unclean details are handled in implementation file
		class Impl;
		std::unique_ptr<Impl> pimpl;
};
