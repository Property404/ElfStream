#pragma once
#include "Range.h"
#include <string>
#include <vector>

/*
 * Remove certain sections from elf file contents
 * and return the snipped contents. Also,
 * "return" via reference which ranges were
 * snipped
 *
 * `ranges` must be empty
 */
std::string scrubElf(const std::string& elf_path, std::vector<Range>& ranges);

/*
 * Fill in scrubbed elf contents with zeros
 * according to ranges
 */
std::string expandScrubbedElf(const std::string& contents, const std::vector<Range>& ranges);
