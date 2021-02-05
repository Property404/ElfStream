#include "catch.hpp"
#include "Socket.h"
#include <vector>
#include <map>

constexpr auto ECHOSERVER_PORT = 4008;

/* This test makes sure we can send and receive simple data */
/* Requires the echo server to be running (should have been taken care of by
 * Makefile*/
TEST_CASE("String echo test", "[Server][Socket][unit]")
{
	const std::vector<std::string> messages = {
		"Hello World!",
		"okay",
		"Hi\0there\0friend",
		"Everybody's\ndoing\rthe Ark",
		"!",
	};

	Socket client;
	client.connect("localhost", ECHOSERVER_PORT);

	for(const auto& message : messages)
	{
		client.send(message);
		REQUIRE(client.receive() == message);
	}
}
