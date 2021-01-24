#include <elfio/elfio.hpp>

using namespace ELFIO;
int main()
{
	const std::string fn = "playground/a.out";
	elfio reader;
	reader.load(fn);

	elfio improved;
	improved.create(reader.get_class(), reader.get_encoding());

	improved.set_os_abi(reader.get_os_abi());
	improved.set_type(reader.get_type());
	improved.set_machine(reader.get_machine());
	improved.set_entry(reader.get_entry());

	for(unsigned i=0;i<reader.segments.size();i++)
	{
		improved.segments.add();
		improved.segments[i]->set_type(reader.segments[i]->get_type());
		improved.segments[i]->set_flags(reader.segments[i]->get_flags());
		improved.segments[i]->set_align(reader.segments[i]->get_align());
		improved.segments[i]->set_virtual_address(reader.segments[i]->get_virtual_address());
		improved.segments[i]->set_physical_address(reader.segments[i]->get_physical_address());
		improved.segments[i]->set_file_size(reader.segments[i]->get_file_size());
		improved.segments[i]->set_memory_size(reader.segments[i]->get_memory_size());
	}

	improved.save("test");
}
