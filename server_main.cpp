#include "ElfStreamServer.h"
#include "common.h"

int main()
{
	ElfStreamServer server;
	server.listen(ELFSTREAM_PORT);
}
