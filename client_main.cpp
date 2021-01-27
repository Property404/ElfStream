#include "Merchant.h"
#include "Agent.h"

int main()
{
	const std::string elf_path = "./playground/hello.elf";
	auto merchant = std::make_shared<Merchant>("localhost", elf_path);

	Agent agent(merchant);
	agent.spawn();
	agent.run();
}
