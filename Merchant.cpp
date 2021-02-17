#include "Merchant.h"
#include "Socket.h"
#include <list>
#include <vector>
#include <sstream>
#include <cassert>
#include <iostream>

// Temporary implementation while we build full structure
// Sucks information out of a local elf file
struct Merchant::Impl
{
	Socket client;
};


Merchant::Merchant(const std::string& host, const std::string& elf_path, int port):pimpl(std::make_unique<Impl>())
{
	pimpl->client.connect(host, port);
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

void* Merchant::entryPoint()
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

std::string Merchant::getBlankElf(std::vector<Range>& ranges)
{
	pimpl->client.send("get_blank_elf");
	const auto blank_elf = pimpl->client.receive();

	pimpl->client.send("get_wiped_ranges");
	auto ranges_serialized = pimpl->client.receive();
	
	// Deserialize ranges
	std::stringstream ss;
	size_t num_ranges;
	ss<<ranges_serialized;
	ss>>num_ranges;
	for(unsigned i=0;i<num_ranges;i++)
	{
		size_t start, size;
		ss<<ranges_serialized;
		ss>>start;
		ss<<ranges_serialized;
		ss>>size;
		ranges.emplace_back(start, size);
	}

	return blank_elf;
}

Merchant::~Merchant() = default;
