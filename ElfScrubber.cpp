#include "ElfScrubber.h"
#include <elfio/elfio.hpp>

using namespace ELFIO;

ElfScrubber::ElfScrubber(const std::string& fn)
{
	reader.load(fn);

	for(unsigned i=0; i<sections.size();i++)
	{
	}
}
