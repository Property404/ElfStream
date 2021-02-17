#include "Merchant.h"
#include "Agent.h"
#include "args/args.hxx"
#include "common.h"
#include <iostream>

int main(const int argc, const char* argv[])
{
	args::ArgumentParser argparser("Elf Stream Client", "");
	args::HelpFlag help(argparser, "help", "Display this help menu", {"help"});

	args::Group required(argparser, "", args::Group::Validators::All);
	args::Positional<std::string> elf_path_arg (required, "Elf file", "Path for elf file located on server");
	args::ValueFlag<std::string> host_arg (required, "host", "Hostname or ip address", {'H', "host"});

	args::ValueFlag<int> port_arg (argparser, "port", "Port", {'p', "port"});

	args::PositionalList<std::string> vargs_arg (argparser, "args", "Arguments to pass to inferior");

	// Don't show that "-- can be used to terminate.." thing
	argparser.helpParams.showTerminator = false;

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

	int port = port_arg?args::get(port_arg):ELFSTREAM_PORT;
	auto merchant = std::make_shared<Merchant>(
		args::get(host_arg),
		args::get(elf_path_arg),
		port
	);
	
	Agent agent(merchant);
	agent.spawn(args::get(vargs_arg));
	agent.run();
}
