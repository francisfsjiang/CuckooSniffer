#include "http/data_processor.hpp"

#include "cuckoo_sniffer.hpp"
#include "util/base64.hpp"
#include "util/file.hpp"
#include "util/mail_process.hpp"
#include "util/function.hpp"
#include "util/buffer.hpp"
#include "collected_data.hpp"

#include "http/parser.hpp"
#include "http/data.hpp"

namespace cs::http {

    std::vector<cs::util::File*> processor(cs::base::CollectedData* collected_data) {
        LOG_DEBUG << "Start HTTP data process.";

        auto data = dynamic_cast<cs::http::CollectedData*>(collected_data);


        std::vector<cs::util::File*> ret;
        return ret;

    }


}