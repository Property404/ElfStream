CFLAGS = -Wall -Wextra -fmax-errors=1 -O0 -g
STATIC_FLAGS = -static-libgcc -static
PIE_FLAGS = -fPIE -pie
PROGRAMS = hello fizzbuzz primes

all: $(PROGRAMS)
% : %.c
	gcc $(CFLAGS) $< -o $@
	gcc $(CFLAGS) $(STATIC_FLAGS) $< -o $@.static
	gcc $(CFLAGS) $(PIE_FLAGS) $< -o $@.pie
	./$@ > $@.output
% : %.cpp
	g++ $(CFLAGS) $< -o $@
	g++ $(CFLAGS) $(STATIC_FLAGS) $< -o $@.static
	g++ $(CFLAGS) $(PIE_FLAGS) $< -o $@.pie
	./$@ > $@.output
clean:
	rm -f $(PROGRAMS)
	rm -f *.o
	rm -f *.elf
	rm -f *.out
	rm -f *.output
	rm -f *.pie
	rm -f *.static
