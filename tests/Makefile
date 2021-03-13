CXXFLAGS = -Wall -Wextra -fmax-errors=1 -O0 -g -I.. -std=c++2a -fconcepts -I../elf-parser
common_objects = ../Server.o ../Socket.o ../Agent.o ../Parser.o\
				 ../inject.o ../FileUtil.o ../scrub.o ../elf-parser/elf_parser.o\
				 ../Merchant.o
unit_test_objects = main.o client-server.o scrub.o integration.o local.o\
					merchant.o parser.o $(common_objects)
echo_server_objects = echo_server.o $(common_objects)

all: unit echo_server samples
unit: $(unit_test_objects) 
	$(CXX) -o unit $(unit_test_objects) $(CXXFLAGS)
echo_server: $(echo_server_objects)
	$(CXX) -o echo_server $(echo_server_objects) $(CXXFLAGS)
.PHONY: samples
samples:
	$(MAKE) -C samples
clean:
	rm -f unit
	rm -f echo_server
	rm -f $(echo_server_objects)
	rm -f $(unit_test_objects)
	$(MAKE) -C samples clean
