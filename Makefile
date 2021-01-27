testdir := tests
common_objects = Socket.o FileUtil.o
client_objects = client_main.o Merchant.o Agent.o inject.o $(common_objects)
server_objects = Parser.o server_main.o Server.o ElfStreamServer.o scrub.o $(common_objects)

CXXFLAGS = -Wall -Wextra -fmax-errors=1 -std=c++17
CFLAGS = -Wall -Wextra -fmax-errors=1 -std=gnu11

# Actual executable
all: esclient esserver
esclient: $(client_objects)
	g++ -o esclient $(client_objects) $(CXXFLAGS)
esserver: $(server_objects)
	g++ -o esserver $(server_objects) $(CXXFLAGS)
clean:
	rm -f *.o
	rm -f esclient
	rm -f esserver
