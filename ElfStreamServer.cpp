#include "ElfStreamServer.h"
#include "scrub.h"
#include "Parser.h"
#include "FileUtil.h"
#include <sstream>
#include <iostream>
#include <stdexcept>

std::string pointerToString(const void* const ptr)
{
	std::stringstream ss;
	ss<<ptr;
	return ss.str();
}

std::string ElfStreamServer::handler(std::string&& message)
{
	// First communication!
	if(parser == nullptr)
	{
		file_name = message;
		parser = std::make_unique<Parser>(file_name);
		return "OK";
	}

	std::stringstream ss;
	ss<<message;

	std::string command;
	ss>>command;

	// Listen to requests and send back
	// Simple protocol we'll probably fix
	// when we have more time
	if(command == "get_text_start")
		return pointerToString(parser->textStart());

	if(command == "get_memory_size")
		return std::to_string(parser->memorySize());

	if(command == "get_memory_start")
		return pointerToString(parser->memoryStart());

	if(command == "get_blank_elf")
	{
		return parser->getBlankElf();
	}

	if(command == "fetch")
	{
		void* address;
		ss>>address;

		AbstractElfAccessor::PatchList patches;
		parser->fetchPatches(address, patches);

		std::string response;
		for(const auto& patch:patches)
			response+=patch.serialize();
		std::cout<<response;

		return response;
	}

	throw std::runtime_error("Unknown command: "+message);
}

