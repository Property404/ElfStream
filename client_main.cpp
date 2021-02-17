#include "Merchant.h"
#include "Agent.h"
#include <iostream>

int main(const int argc, const char* argv[])
{
	if(argc < 2)
	{
		std::cerr<<"Need argument"<<std::endl;
		return EXIT_FAILURE;
	}
	const std::string elf_path = argv[1];

	auto merchant = std::make_shared<Merchant>("localhost", elf_path);

	Agent agent(merchant);
	agent.spawn();
	agent.run();
}
