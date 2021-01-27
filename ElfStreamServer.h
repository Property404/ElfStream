#include "Server.h"
#include "Parser.h"
#include <memory>

class ElfStreamServer: public Server
{
	std::string file_name;
	std::unique_ptr<Parser> parser;
	virtual std::string handler(std::string&&) override;
};
