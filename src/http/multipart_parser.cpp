#include "multipart_parser.hpp"

#include <boost/algorithm/string.hpp>

#include "cuckoo_sniffer.hpp"
#include "util/file.hpp"
#include "util/buffer.hpp"

#include "mime/Mime.h"

namespace cs::http {

    using namespace std;
    using namespace cs::util;

     int multipart_parse(const cs::util::Buffer& buffer, const string& boundary, const string& content_type, vector<cs::util::File*>& file_vec) {
        CMimeMessage mail;
        mail.SetBoundary(boundary.data());

        auto new_buffer = new cs::util::Buffer(buffer);
        new_buffer->shrink(2);
        new_buffer->data()[0] = '\r';
        new_buffer->data()[1] = '\n';
        new_buffer->set_reader_pos(0);

        int nLoadedSize = mail.Load(new_buffer->data_to_read(), (int)new_buffer->size());

// Iterate all the descendant body parts
        CMimeBody::CBodyList bodies;
        int nCount = mail.GetBodyPartList(bodies);
        if (nCount < 1) {
            return -1;
        }
        CMimeBody::CBodyList::const_iterator it;
        for (it=bodies.begin(); it!=bodies.end(); it++)
        {
            CMimeBody* pBP = *it;

            // Iterate all the header fields of this body part:
            CMimeHeader::CFieldList& fds = pBP->Fields();
            CMimeHeader::CFieldList::const_iterator itfd;
            std::string file_name = "";
            for (itfd=fds.begin(); itfd!=fds.end(); itfd++)
            {
                const CMimeField& fd = *itfd;
                if(strcmp("Content-Disposition", fd.GetName()) == 0) {
                    string head;
                    map<string, string> params;
                    auto ret = parse_parameters(fd.GetValue(), head, params);
                    LOG_DEBUG << head;
                    for(auto i: params) {
                        LOG_DEBUG << i.first << "   " << i.second;
                    }
                    auto  iter = params.find("filename");
                    if (iter != params.end()) {
                        file_name = iter->second;
                    }
                }

            }
//            LOG_DEBUG << pBP->GetContentType();
            auto f = new File();
//            if (params.find("filename") != params.end()) {
//                f->set_name(*params.find("filename"));
//            }
            f->write((const char*)(pBP->GetContent()), pBP->GetContentLength());
            if (file_name.empty()) {
                file_name = f->get_md5();
            }
            LOG_DEBUG << "Get file: " << file_name << ", md5: "  << f->get_md5();
            f->set_name(file_name);
            file_vec.push_back(f);
        }
        return 0;
    }


     int parse_parameters(const string& str, string& head, map<string, string>& parameters) {
        vector<string> strs;
        boost::split(strs, str, boost::is_any_of("; =\""), boost::token_compress_on);

        if (strs.size() < 1) {
            return -1;
        }

        head = strs[0];
        for (int i = 1; i+1 < strs.size(); i+=2) {
            parameters.insert(make_pair(strs[i], strs[i+1]));
        }
        return 0;
    }

}
