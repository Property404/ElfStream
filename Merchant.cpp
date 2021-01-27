#include "common.h"
#include "Merchant.h"
#include "Socket.h"
#include <elfio/elfio.hpp>
#include <list>
#include <vector>
#include <sstream>
#include <cassert>
#include <iostream>

static constexpr ssize_t BLOCK_SIZE = 0x1000;

// Temporary implementation while we build full structure
// Sucks information out of a local elf file
struct Merchant::Impl
{
	Socket client;
};


Merchant::Merchant(const std::string& host, const std::string& elf_path):pimpl(std::make_unique<Impl>())
{
	pimpl->client.connect(host, ELFSTREAM_PORT);
	pimpl->client.send(elf_path);
	pimpl->client.receive();
}

void Merchant::fetchPatches(const void* exact_address, Merchant::PatchList& patches)
{
	std::stringstream ss;
	ss<<"fetch ";
	ss<<exact_address;
	pimpl->client.send(ss.str());
	std::string response = pimpl->client.receive();

	auto toNumeric = [](std::string s){
		std::stringstream ss;
		ss<<s;
		size_t res;
		ss>>res;
		return res;
	};

	for(unsigned i=0;i<response.size();i++)
	{
		Patch patch;

		// Get start
		std::string start_string = "";
		while(response[i] != ' '){start_string+=response[i];i++;}
		patch.start = toNumeric(start_string);
		i++;

		// Get size
		std::string size_string = "";
		while(response[i] != ' '){size_string+=response[i];i++;}
		patch.size = toNumeric(size_string);
		i++;

		for(unsigned j=0;j<patch.size;j++,i++)
		{
			patch.content+=response[i];
		}

		patches.push_back(patch);
	}
}

void* Merchant::textStart()
{
	pimpl->client.send("get_text_start");
	auto response = pimpl->client.receive();

	std::stringstream ss;
	ss<<response;
	void* address;
	ss>>address;
	return address;
}

void* Merchant::memoryStart()
{
	pimpl->client.send("get_memory_start");
	auto response = pimpl->client.receive();

	std::stringstream ss;
	ss<<response;
	void* address;
	ss>>address;
	return address;
}

size_t Merchant::memorySize()
{
	pimpl->client.send("get_memory_size");
	auto response = pimpl->client.receive();

	std::stringstream ss;
	ss<<response;
	size_t address;
	ss>>address;
	return address;
}

std::string Merchant::getBlankElf()
{
	pimpl->client.send("get_blank_elf");
	auto response = pimpl->client.receive();
	return response;
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
