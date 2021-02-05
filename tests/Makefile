CXXFLAGS = -Wall -Wextra -fmax-errors=1 -O0 -g -I.. -std=c++17
common_objects = ../Server.o ../Socket.o ../Agent.o ../Parser.o ../inject.o ../FileUtil.o ../scrub.o
unit_test_objects = main.o client-server.o basic.o $(common_objects)
echo_server_objects = echo_server.o $(common_objects)

unit: $(unit_test_objects) echo_server
	g++ -o unit $(unit_test_objects) $(CXXFLAGS)
echo_server: $(echo_server_objects)
	g++ -o echo_server $(echo_server_objects) $(CXXFLAGS)
clean:
	rm -f unit
	rm -f echo_server
	rm -f $(echo_server_objects)
	rm -f $(unit_test_objects)
