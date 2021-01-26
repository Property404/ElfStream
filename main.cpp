#include "Merchant.h"
#include "Agent.h"
#include "scrub.h"
#include "FileUtil.h"

int main()
{
	const std::string good_path = "./playground/hello.elf";
	const std::string bad_path = "/tmp/__scrub";

	if(FileUtil::fileExists(bad_path))
		FileUtil::destroyFile(bad_path);

	scrub_elf(good_path, bad_path);

	auto merchant = std::make_shared<Merchant>(good_path);

	Agent agent(merchant);
	agent.spawn(bad_path);
	agent.run();
}
