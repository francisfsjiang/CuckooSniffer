#include <iostream>

#include "imap/imap_data_processor.hpp"
#include "imap/imap_sniffer_data.hpp"
#include "util/base64.hpp"
#include "util/file.hpp"
#include "util/mail_process.hpp"


int IMAPDataProcessor::process(const SnifferData& sniffer_data_arg) {

    const IMAPSnifferData& sniffer_data = (const IMAPSnifferData&)sniffer_data_arg;
    std::string data = sniffer_data.get_data();

    std::cout << "stat process imap data" << std::endl;
//    std::cout << data << std::endl << std::endl;

    static const std::regex departer("\\* \\d* FETCH \\(UID \\d* RFC822.SIZE \\d* BODY\\[\\] \\{\\d*\\}([\\s^\\S]*?)\n\\)\r\n)");
    std::smatch match;
    while(regex_search(data, match, departer))
    {
        mail_process(match.str(1));
        data = match.suffix();
    }

    return 1;

}


IMAPDataProcessor::~IMAPDataProcessor() { }
