#include "Merchant.h"
#include "Agent.h"

int main()
{
	const std::string good_path = "./playground/good.elf";
	const std::string bad_path = "./playground/bad.elf";
	auto merchant = std::make_shared<Merchant>(good_path);
	Agent agent(merchant);
	agent.spawn(bad_path);
	agent.run();
}
