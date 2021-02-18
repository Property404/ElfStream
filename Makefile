testdir := tests
common_objects = Socket.o FileUtil.o scrub.o elf-parser/elf_parser.o 
client_objects = client_main.o Merchant.o Agent.o inject.o $(common_objects)
server_objects = Parser.o server_main.o Server.o ElfStreamServer.o $(common_objects)

CXXFLAGS = -Wall -Wextra -fmax-errors=1 -std=c++20 -Ielf-parser
CFLAGS = -Wall -Wextra -fmax-errors=1 -std=gnu11

# Actual executables
all: esclient esserver
esclient: $(client_objects)
	g++ -o esclient $(client_objects) $(CXXFLAGS)
esserver: $(server_objects)
	g++ -o esserver $(server_objects) $(CXXFLAGS)

# Start test servers and run
test: $(testdir) esserver
	./esserver &
	cd $(testdir) && ./echo_server &
	cd $(testdir) && ./unit || true
	killall esserver && echo "ElfStream Server killed"
	killall echo_server && echo "Echo Server killed"
$(testdir):
	$(MAKE) -C $@

# Update objecs on header change
$(objects): *.h

clean:
	rm -f *.o
	rm -f esclient
	rm -f esserver
	$(MAKE) -C $(testdir) clean

.PHONY: test $(testdir)
