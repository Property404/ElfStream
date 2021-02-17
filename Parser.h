#pragma once
#include <string>
#include <memory>
#include <list>
#include "AbstractElfAccessor.h"

class Parser : public AbstractElfAccessor
{
	public:
		Parser(const std::string& elf_path);
		~Parser();

		// Fetch patches within block containing `address`
		// `Address` may be any address inside a valid block
		// `Patches` is filled with patches for the block
		void fetchPatches(const void* address, PatchList& patches) override;

		// Returns address of first block
		void* memoryStart() override;

		// Returns first address of executable memory
		// (not block aligned)
		void* entryPoint() override;

		// Returns total size of relevant regions
		// (Regions that merchant transfers and agent protects)
		size_t memorySize() override;

		// Get the clipped version of elf file
		std::string getBlankElf(std::vector<Range>& ranges) override;

		// Translation between ELF offsets and vmem addressses
		size_t translateAddressToOffset(uintptr_t address) const;
		uintptr_t translateOffsetToAddress(size_t offset) const;

	protected:
		// Get ELF file path
		std::string getPath() const;

	private:
		// Filthy, dirty, unclean details are handled in implementation file
		class Impl;
		std::unique_ptr<Impl> pimpl;

};
