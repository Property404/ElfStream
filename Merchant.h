#pragma once
#include <string>
#include <memory>

class Merchant
{
	public:
		Merchant(const std::string& elf_path);
		~Merchant();

		// Fetch block containing `address`
		// `Address` may be any address inside a valid block
		std::string fetchBlockOf(const void* address);
		
		void* alignToBlockStart(const void* address) const;
		size_t getBlockSize() const noexcept;

	private:
		// Filthy, dirty, unclean details are handled in implementation file
		class Impl;
		std::unique_ptr<Impl> pimpl;
};
