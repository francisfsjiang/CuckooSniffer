#include "smtp/data_processor.hpp"

#include "smtp/collected_data.hpp"
#include "util/base64.hpp"
#include "util/file.hpp"
#include "util/mail_process.hpp"

namespace cs {
namespace smtp {

int DataProcessor::process(cs::base::CollectedData* collected_data_arg) {

    CollectedData &sniffer_data = *dynamic_cast<CollectedData*>(collected_data_arg);
    util::mail_process(sniffer_data.get_data());

    return 1;

}


DataProcessor::~DataProcessor() {}

}
}