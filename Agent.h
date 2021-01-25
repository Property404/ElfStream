#include <memory>
#include <string>
#include "Merchant.h"
class Agent
{
	struct Impl;
	std::unique_ptr<Impl> pimpl;

	std::shared_ptr<Merchant> merchant;
	public:
		Agent(std::shared_ptr<Merchant>);
		void spawn(const std::string&);
		void run();
		~Agent();
};
