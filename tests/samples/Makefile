CFLAGS = -Wall -Wextra -fmax-errors=1 -O0 -g
LDFLAGS += -static-libgcc -static
PROGRAMS = hello fizzbuzz primes

all: $(PROGRAMS)
% : %.c
	gcc $(CFLAGS) $(LDFLAGS) $< -o $@
	./$@ > $@.output
% : %.cpp
	g++ $(CFLAGS) $(LDFLAGS) $< -o $@
	./$@ > $@.output
clean:
	rm -f $(PROGRAMS)
	rm -f *.o
	rm -f *.elf
	rm -f *.out
	rm -f *.output
