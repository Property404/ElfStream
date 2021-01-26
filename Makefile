testdir := tests
objects = main.o Merchant.o Agent.o inject.o scrub.o FileUtil.o
CXXFLAGS = -Wall -Wextra -fmax-errors=1 -std=c++17
CFLAGS = -Wall -Wextra -fmax-errors=1 -std=gnu11

# Actual executable
a.out: $(objects)
	g++ -o a.out $(objects) $(CXXFLAGS)
clean:
	rm -f *.o
	rm -f a.out
