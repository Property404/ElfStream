#pragma once
#include <string>
#include <memory>
#include <list>

class Merchant
{
	public:
		Merchant(const std::string& elf_path);
		~Merchant();

		struct Patch{
			size_t start;
			size_t size;
			std::string content;
		};
		using PatchList = std::list<Patch>;
		// Fetch patches within block containing `address`
		// `Address` may be any address inside a valid block
		// `Patches` is filled with patches for the block
		void fetchPatches(const void* address, PatchList& patches);

		// Returns address of first block
		void* memoryStart();

		// Returns first address of executable memory
		// (not block aligned)
		void* textStart();

		// Returns total size of relevant regions
		// (Regions that merchant transfers and agent protects)
		size_t memorySize();

		// Page size is determined by local OS, so this doesn't talk to the server
		void* alignToBlockStart(const void* address) const;
		size_t getBlockSize() const noexcept;

	private:
		// Filthy, dirty, unclean details are handled in implementation file
		class Impl;
		std::unique_ptr<Impl> pimpl;
};
