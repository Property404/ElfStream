#include "scrub.h"
#include <vector>
#include <string>

void scrub_elf(const std::string& elf_path, const std::string& output_path)
{
	const std::string sections[] = {".rodata", ".text"};
	std::string command = "objcopy";

	for(const auto& section : sections)
		command += " --update-section "+section+"=/dev/null ";

	command += elf_path + " " + output_path;
	system(command.c_str());
}
