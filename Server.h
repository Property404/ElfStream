#pragma once
#include <memory>
#include <string>
/*
 * TCP Server
 */
class Server
{
	// Handler to be used on receiving data
	// To be overridden by child class
	virtual std::string handler(std::string&&) = 0;

	public:
		Server() = default;

		// Listen on port `port`
		// Continuously execute handler on received data
		Server& listen(int port);

};
