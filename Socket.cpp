#include "Socket.h"
#include "warning.h"
#include <stdexcept>
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
// Hints struct used in Socket::bind()
// and Socket::connect()
static const addrinfo hints = {
	.ai_flags = 0,
	.ai_family=AF_UNSPEC,
	.ai_socktype=SOCK_STREAM,
	.ai_protocol = 0,
	.ai_addrlen = 0,
	.ai_addr = nullptr,
	.ai_canonname = nullptr,
	.ai_next = nullptr
};

bool Socket::isValid() const noexcept
{
	return (this->socket_fd > 0);
}

void Socket::assertValidity() const
{
	if(!isValid())
		throw std::runtime_error("Socket is uninitialized");
}
void Socket::assertInvalidity() const
{
	if(isValid())
	{
		throw std::runtime_error("Socket is already initialized");
	}
}

static char buffer[1024];// TODO: UNACCEPTABLE
std::string Socket::receive() const
{
	assertValidity();

	ssize_t length;
	std::string message;

	length = recv(this->socket_fd, &buffer, sizeof(buffer), 0);

	if(length>0)
		message += std::string(buffer, length);
	if(length == 0)
		throw Socket::PeerDisconnect("Peer disconnected");
	if(length<0)
		throw std::runtime_error("Receive failed");

	while(length == sizeof(buffer))
	{
		usleep(1000);
		length = recv(this->socket_fd, &buffer, sizeof(buffer), MSG_DONTWAIT);
		if(length>0 || length == EAGAIN)
		{
			message += std::string(buffer, length);
		}
	}
	return message;
}

void Socket::send(std::string message) const
{
	assertValidity();

	const ssize_t bytes_sent = ::send(this->socket_fd, message.c_str(), message.size(), 0);
	if(bytes_sent != static_cast<ssize_t>(message.size()))
		throw std::runtime_error("Send failed");
}

int Socket::accept()
{
	assertValidity();

	sockaddr_storage client_address;
	socklen_t address_size = sizeof(client_address);
	const auto new_socket_fd =
		::accept(this->socket_fd, (sockaddr*)&client_address, &address_size);

	return new_socket_fd;
}

void Socket::listen(int backlog) const
{
	assertValidity();
	if(::listen(this->socket_fd, backlog) < 0)
	{
		throw std::runtime_error("Socket failed to listen on TCP port");
	}
}

void Socket::bind(int port)
{
	assertInvalidity();

	int cstatus;// return val of C API functions
	addrinfo* server_info_list = nullptr;

	if((cstatus = getaddrinfo(nullptr, std::to_string(port).c_str(), &hints, &server_info_list)))
	{
		throw std::runtime_error(std::string("getaddrinfo failed: ")+std::to_string(cstatus));
	}

	// bind to whoever will let us
	auto server_info = server_info_list;
	for ( ; server_info != nullptr; server_info = server_info->ai_next)
	{
		this->socket_fd = 
			socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);

		if(! this->isValid())
		{
			warning(std::string("Failed to create socket: ")+std::to_string(this->socket_fd));
			continue;
		}

		// Prevent annoying "address in use" error

		// for setting SO_REUSEADDR to true
		int optval = 1;

		if((cstatus = setsockopt(this->socket_fd,
				SOL_SOCKET,
				SO_REUSEADDR,
				&optval,
				sizeof(optval))))
		{
			freeaddrinfo(server_info_list);
			throw std::runtime_error(std::string("setsockopt failed: ")+std::to_string(cstatus));
		}

		
		if(::bind(this->socket_fd, server_info->ai_addr, server_info->ai_addrlen))
		{
			this->close();
			warning(std::string("bind() failed. errno=")+std::to_string(errno));
			continue;
		}
		break;
	}
	freeaddrinfo(server_info_list);
	if(server_info == nullptr)
		throw std::runtime_error("Failed to bind() socket");
}

void Socket::connect(std::string host, int port)
{
	assertInvalidity();
	int cstatus;
	addrinfo* server_info_list = nullptr;

	if((cstatus = getaddrinfo(host.c_str(), std::to_string(port).c_str(), &hints, &server_info_list)))
	{
		throw std::runtime_error(std::string("getaddrinfo() failed: ")+std::to_string(cstatus));
	}

	auto server_info = server_info_list;
	for ( ; server_info != nullptr; server_info = server_info->ai_next)
	{
		this->socket_fd = 
			socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);

		if(!this->isValid())
		{
			warning(std::string("Failed to create socket: ")+std::to_string(this->socket_fd));
			continue;
		}

		if(::connect(this->socket_fd, server_info->ai_addr, server_info->ai_addrlen))
		{
			this->close();
			warning(std::string("connect() failed. errno=")+std::to_string(errno));
			continue;
		}
		break;
	}
	freeaddrinfo(server_info_list);
	if(server_info == nullptr)
		throw std::runtime_error("Failed to connect() socket");
}

void Socket::close()
{
	assertValidity();
	::close(this->socket_fd);
	this->socket_fd = 0;
}

Socket::Socket(Socket&& rhs)
{
	this->socket_fd = rhs.socket_fd;
	rhs.socket_fd = 0;
}
Socket::~Socket()
{
	if(this->isValid())
		::close(this->socket_fd);
}