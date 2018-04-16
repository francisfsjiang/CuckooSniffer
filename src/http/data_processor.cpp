#include "http/data_processor.hpp"

#include "cuckoo_sniffer.hpp"
#include "util/base64.hpp"
#include "util/file.hpp"
#include "util/mail_process.hpp"
#include "util/function.hpp"
#include "util/buffer.hpp"
#include "collected_data.hpp"

namespace cs::http {

    std::vector<cs::util::File*> processor(cs::base::CollectedData* collected_data) {
        LOG_DEBUG << "Start HTTP data process.";

        auto cdata = dynamic_cast<cs::http::CollectedData*>(collected_data);
        const std::string& data = std::string(cdata->client_buffer_->data_to_read(), cdata->client_buffer_->size());

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

    }


}