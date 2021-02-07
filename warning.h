#pragma once
#include <iostream>
#include <string>

// Uniform way of giving out warnings
inline void warning(std::string val)
{
	std::cerr<<"#Warning: "<<val<<std::endl;
}
