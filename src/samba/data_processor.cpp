#include <cuckoo_sniffer.hpp>

#include "cuckoo_sniffer.hpp"
#include "samba/collected_data.hpp"
#include "samba/data_processor.hpp"
#include "util/base64.hpp"
#include "util/file.hpp"
#include "util/mail_process.hpp"

namespace cs {
namespace samba {

int DataProcessor::process(cs::base::CollectedData* sniffer_data_ptr) {

    LOG_TRACE << "SAMBA data process";
    CollectedData& sniffer_data = *dynamic_cast<CollectedData*>(sniffer_data_ptr);
    cs::util::File* file = sniffer_data.get_data();

    LOG_INFO << "SAMBA file name " << file -> get_name();
    LOG_INFO << "SAMBA file size " << file -> get_size();
    LOG_INFO << "SAMBA file md5 " << file -> get_md5();


    return 1;

}


DataProcessor::~DataProcessor() {}

}
}