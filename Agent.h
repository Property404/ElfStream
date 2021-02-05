#include <memory>
#include <string>
#include "Merchant.h"
class Agent
{
	struct Impl;
	std::unique_ptr<Impl> pimpl;

	// The Elf Accessor what's used to access elf file
	// information
	std::shared_ptr<AbstractElfAccessor> merchant;

	// mprotect() needs to be injected somewhere in the client
	// Use this to set up that area on the stack
	void* createInjectionSite();
	public:
		Agent(std::shared_ptr<AbstractElfAccessor>);
		~Agent();

		// Set up to send inferior's output to file
		void redirectOutput(std::string file_name);

		// Spawn inferior
		void spawn();
		
		// Run inferior
		void run();
};
