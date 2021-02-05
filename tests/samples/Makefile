CFLAGS = -Wall -Wextra -fmax-errors=1 -O0 -g
LDFLAGS += -static-libgcc -static

all: hello fizzbuzz
hello: hello.o
	gcc -o hello.elf hello.o $(LDFLAGS)
fizzbuzz: fizzbuzz.o
	gcc -o fizzbuzz.elf fizzbuzz.o $(LDFLAGS)
clean:
	rm -f *.o
	rm -f *.elf
	rm -f *.out
