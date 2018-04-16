#include <cuckoo_sniffer.hpp>

#include "cuckoo_sniffer.hpp"
#include "samba/data_processor.hpp"
#include "util/base64.hpp"
#include "util/file.hpp"
#include "util/mail_process.hpp"
#include "samba/collected_data.hpp"

namespace cs::samba {
    std::vector<cs::util::File*> processor(cs::base::CollectedData* collected_data) {

        auto data = dynamic_cast<cs::samba::CollectedData*>(collected_data);
        LOG_TRACE << "SAMBA data process";
        cs::util::File* file = data->file_;

        LOG_INFO << "SAMBA file name " << file -> get_name();
        LOG_INFO << "SAMBA file size " << file -> get_size();
        LOG_INFO << "SAMBA file md5 " << file -> get_md5();

        return {file};

    }

}