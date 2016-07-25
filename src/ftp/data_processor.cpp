#include "ftp/data_processor.hpp"
#include "ftp/sniffer_data.hpp"
#include "util/base64.hpp"
#include "util/file.hpp"
#include "util/mail_process.hpp"

namespace cs::ftp {

int DataProcessor::process(const cs::base::SnifferData &sniffer_data_arg) {

    const SnifferData &sniffer_data = (const SnifferData &) sniffer_data_arg;
    mail_process(sniffer_data.get_data());

    return 1;

}


DataProcessor::~DataProcessor() {}

}