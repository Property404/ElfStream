#include "scrub.h"

void scrub_elf(const std::string& elf_path, const std::string& output_path)
{
	std::string command = "objcopy --update-section .text=/dev/null ";
	command += elf_path + " " + output_path;
	system(command.c_str());
}
