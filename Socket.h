#pragma once
#include <string>
#include <stdexcept>
// Represents a TCP socket
//
// This class contains all the logic pertaining to interacting with BSD
// sockets
class Socket{
	static constexpr auto DEFAULT_BACKLOG=20;

	// Actual unix socket file descriptor
	int socket_fd = 0;

	// Make sure we're in a valid state, throw otherwise
	void assertValidity()const;
	// Make sure we're in an INvalid state, throw otherwise
	void assertInvalidity()const;

	public:
	Socket(){}
	Socket(int fd):socket_fd(fd){}

	bool isValid()const noexcept;


	// Just wrappers around send(), recv(), etc
	void send(std::string message)const;
	// This throws a PeerDisconnect exception if peer disconnects
	std::string receive()const;
	void listen(int backlog = Socket::DEFAULT_BACKLOG) const;
	// accept() might store address info, so it's left not const
	int accept();
	// Prematurely closes socket, but sets file descriptor to zero
	// so destructor won't close twice
	void close();

	// Bind and connect are more like initialization functions here
	// Neither of these are necessary/available if the socket has
	// already been initialized with a file descriptor 
	//
	// Bind initializes a server listener socket
	void bind(int port);
	// Connect initializes client socket
	void connect(std::string host, int port);

	// Copying is unecessary for this application
	// and causes nothing but trouble!
	Socket(const Socket&) = delete;
	Socket& operator=(const Socket&) = delete;
	Socket& operator=(Socket&&) = delete;

	// Moving is fine, however
	Socket(Socket&&);

	// To let the caller know peer has shutdown
	class PeerDisconnect : std::runtime_error
	{
		public:
			PeerDisconnect(const std::string& message): runtime_error(message){}
	};

	~Socket();
};
