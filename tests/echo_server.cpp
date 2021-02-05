#include "Server.h"
#include <iostream>
constexpr auto ECHOSERVER_PORT = 4008;

/* Simple string echo-back server */
class EchoServer : public Server
{
	public:
		std::string handler(std::string&& message)
		{
			return message;
		}
};
int main()
{
	EchoServer server;
	server.listen(ECHOSERVER_PORT);
}
