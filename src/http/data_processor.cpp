#include "http/data_processor.hpp"

#include "cuckoo_sniffer.hpp"
#include "http/collected_data.hpp"
#include "util/base64.hpp"
#include "util/file.hpp"
#include "util/mail_process.hpp"
#include "util/function.hpp"

namespace cs {
namespace http {

int DataProcessor::process(const cs::base::CollectedData &sniffer_data_arg) {
    LOG_DEBUG << "Start HTTP data process.";

    const CollectedData &sniffer_data = (const CollectedData &) sniffer_data_arg;
    const std::string& data = sniffer_data.get_data();

    std::vector<std::string> str_vec;

    size_t  last_pos = 0, new_pos;
    std::string tmp;
    while (1) {
        new_pos = data.find("\r\n", last_pos);
        tmp= data.substr(last_pos, new_pos - last_pos);
        str_vec.push_back(tmp);
        if (new_pos == std::string::npos) {
            break;
        }
        else if(new_pos == last_pos) {
            str_vec.push_back(data.substr(new_pos + 2, data.size()));
            break;
        }
        else {
            last_pos = new_pos + 2;
        }
    }

    LOG_DEBUG << "Split HTTP into part:" << str_vec.size();

    for (const auto &iter: str_vec) {
        if (iter.size() < 50 && iter.size() > 0)
            LOG_TRACE << iter;
    }

    std::string file_content;
    bool flag = false;
    for (int i = 0; i < str_vec.size(); ++i) {
        if (str_vec[i].size() == 0) {
            file_content = str_vec[i + 1];
        }
    }

    LOG_DEBUG << "Get HTTP Post file size:" << file_content.size();

    return 1;

}


DataProcessor::~DataProcessor() {}

}
}