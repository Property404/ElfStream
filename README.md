# Elf Stream
Stream/lazy-load elf files over a network  

## How it Works
Client received a reduced version of the ELF file from the server.  
Upon each access to a region of code or data that the client doesn't have,
the client requests that that region from the server  

## Building
Requires a C++20 compiler  
`git clone --recursive git@github.com/Property404/ElfStream`  
`make`  
`make test`  

## Usage
On Server: `./esserver`  
On Client: `./esclient -H example.com path/to/elf/file`  

## Limits  
* Limited to 64-bit ELF files
* Limited to GNU/Linux on x86-64  
* Does not load shared libraries not on system
* Does not copy resources(images, configs, etc) over network
