#include "Server.h"
#include "warning.h"
#include "Socket.h"
#include <unistd.h>
#include <stdexcept>

Server& Server::listen(int port)
{
	Socket listener;
	listener.bind(port);
	listener.listen();

	// Wait for connections
	while(true)
	{
		Socket connection(listener.accept());

		if(!connection.isValid())
		{
			warning("Failed to accept incoming connection");
			continue;
		}

		const auto pid = fork();

		if(pid == 0)
		{
			// Newborn child acts as response server to client
			// then kills itself when the client hangs up
			
			listener.close();

			while(true)
			{
				try
				{
					auto message_from_client = connection.receive();
					const auto message_to_client = this->handler(std::move(message_from_client));
					connection.send(message_to_client);
				}
				catch(const Socket::PeerDisconnect&)
				{
					break;
				}
				catch(const std::exception& e)
				{
					warning(e.what());
				}
			}
			exit(0);
		}
	}
	return *this;
}
