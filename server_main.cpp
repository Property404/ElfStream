#include "ElfStreamServer.h"
#include "common.h"
#include "args/args.hxx"
#include <iostream>

int main(const int argc, const char* argv[])
{
	args::ArgumentParser argparser("Elf Stream Server", "");
	args::HelpFlag help(argparser, "help", "Display this help menu", {"help"});
	args::ValueFlag<int> port_arg (argparser, "port", "Port", {'p', "port"});

	try
	{
		argparser.ParseCLI(argc, argv);
	}
	catch(const args::Help&)
	{
		std::cout<<argparser;
		return EXIT_SUCCESS;
	}
	// Some args related error
	catch (const args::ParseError& e)
	{
		std::cerr << e.what() << std::endl;
		std::cerr << argparser;
		return EXIT_FAILURE;
	}
	// Some args validation related error
	catch (const args::ValidationError& e)
	{

		std::cerr << e.what() << std::endl;
		std::cerr << argparser;
		return EXIT_FAILURE;
	}

	const int port =
		port_arg?args::get(port_arg):ELFSTREAM_PORT;

	ElfStreamServer server;
	std::cout<<"Launching server on port "<<port<<std::endl;
	server.listen(port);
}
