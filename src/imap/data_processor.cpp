#include "imap/data_processor.hpp"

#include <iostream>

#include "util/file.hpp"
#include "util/buffer.hpp"
#include "util/mail_process.hpp"

namespace cs::imap {


	std::vector<cs::util::File*> processor(util::Buffer* , util::Buffer* server_data) {
		std::string data = std::string(server_data->data_to_read(), server_data->size());

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


	}

}