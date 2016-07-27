#include "imap/data_processor.hpp"

#include <iostream>

#include "imap/collected_data.hpp"
#include "util/file.hpp"
#include "util/mail_process.hpp"

namespace cs {
namespace imap {

int DataProcessor::process(const cs::base::CollectedData& sniffer_data_arg) {

    const cs::imap::CollectedData& sniffer_data = (const CollectedData&)sniffer_data_arg;
    std::string data = sniffer_data.get_data();

    std::cout << "stat process imap data" << std::endl;
    //std::cout << data << std::endl << std::endl;

    static const std::regex departer("\\* \\d* FETCH \\(UID \\d* (?:RFC822.SIZE \\d* )?BODY\\[\\] \\{\\d*\\}([\\s^\\S]*?)\n\\)\r\n");
    std::smatch match;
	try
	{
		while (regex_search(data, match, departer))
		{
			util::mail_process(match.str(1));
			data = match.suffix();
		}

	}
	catch (const std::exception&)
	{
		std::cerr << "Regex error" << std::endl;
	}
    
    return 1;

}


DataProcessor::~DataProcessor() {

}

}
}