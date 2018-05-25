#include "smtp/sniffer.hpp"

#include <boost/algorithm/string.hpp>
#include "mime/Mime.h"

#include "cuckoo_sniffer.hpp"
#include "sniffer_manager.hpp"
#include "util/buffer.hpp"
#include "util/function.hpp"
#include "util/file.hpp"
#include "util/mail_process.hpp"

namespace cs::smtp {

    using namespace std;
    using namespace cs::util;
    using namespace cs::base;

    bool char_cmp(const char* s1, const char* s2, size_t len) {
        for(int i = 0; i < len; ++i) {
//            cout << (s1[i]!=s2[i]) << " ";
            if (s1[i]!=s2[i])
                return false;
        }
//        cout << endl;
        return true;
    }


    int parse_parameters(const string& str, string& head, map<string, string>& parameters) {
        vector<string> strs;
        boost::split(strs, str, boost::is_any_of("; =\""), boost::token_compress_on);

        if (strs.size() < 1) {
            return -1;
        }

        head = strs[0];
        auto trim_filter = boost::algorithm::is_any_of(" \r\n\t");
        for (int i = 1; i+1 < strs.size(); i+=2) {
            string k = strs[i];
            string v = strs[i+1];
            boost::algorithm::trim_if(k, trim_filter);
            boost::algorithm::trim_if(v, trim_filter);
            parameters.insert(make_pair(k, v));
        }
        return 0;
    }

    static const std::set<std::string> target_file_type = {
            "application/octet-stream",
            "application/zip",
            "application/x-msdownload",
            "text/x-script.phyton"
    };

    int multipart_parse(const Buffer& buffer, const StreamIdentifier& stream_id) {
        CMimeMessage mail;

        int nLoadedSize = mail.Load(buffer.data_to_read(), (int)buffer.size());
        vector<File*> file_vec;
// Iterate all the descendant body parts
        CMimeBody::CBodyList bodies;
//        int nCount = mail.GetBodyPartList(bodies);
        int nCount = mail.GetAttachmentList(bodies);
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
            string content_type = "";
            string file_name = "";
            for (itfd=fds.begin(); itfd!=fds.end(); itfd++)
            {
                const CMimeField& fd = *itfd;
//                LOG_TRACE << fd.GetName() << " : " << fd.GetValue();
                if(strcmp("Content-Type", fd.GetName()) == 0) {
                    string head;
                    map<string, string> params;
                    auto ret = parse_parameters(fd.GetValue(), head, params);
                    content_type = head;
                }
                else if(strcmp("Content-Disposition", fd.GetName()) == 0) {
                    string head;
                    map<string, string> params;
                    auto ret = parse_parameters(fd.GetValue(), head, params);
                    auto iter = params.find("filename");
                    if (iter != params.end()) {
                        file_name = iter->second;
                    }
                }

            }
//            LOG_DEBUG << "Len: " << pBP->GetContentLength();
//            LOG_DEBUG << "Encoding: " << pBP->GetTransferEncoding();
            LOG_DEBUG << "Type: " << content_type;
            LOG_DEBUG << "Filename: " << file_name;
            if (target_file_type.find(content_type) != target_file_type.end()) {
                auto f = new File();
                string file_name = "";
//            if (params.find("filename") != params.end()) {
//                f->set_name(*params.find("filename"));
//            }
                f->write((const char*)(pBP->GetContent()), pBP->GetContentLength());
                if (file_name.empty()) {
                    file_name = f->get_md5();
                }
                LOG_DEBUG << "Get file: " << file_name << ", md5: "  << f->get_md5();
                f->set_name(file_name);

                std::string info = "";
                auto from =  mail.GetFrom();
                if (from)
                    info += string(";From:") + mail.GetFrom();
                auto to =  mail.GetTo();
                if (to)
                    info += string(";To:") + mail.GetTo();
                auto cc =  mail.GetCc();
                if (cc)
                    info += string(";Cc:") + mail.GetCc();
                auto subject =  mail.GetSubject();
                if (subject)
                    info += string(";Subject:") + mail.GetSubject();

                f->set_attached_info(info);
                f->set_ip_info(stream_id);
                file_vec.push_back(f);
            }
        }

        submit_files_and_delete(file_vec);
        return 0;
    }

    void Sniffer::on_client_payload(PayloadVector payload, size_t payload_size) {
        LOG_TRACE << "SMTP client data size :" << payload_size;
//        LOG_TRACE << (char*)payload;
//        for(int i = 0;i<payload_size;++i) {
//            cout << hex << (int)payload[i] << " ";
//        }
//        cout << endl;
        if (quitted_) {
            return;
        }
        if (!start_data_ ) {
            if(payload_size >= 6 && char_cmp((char*)payload, "DATA\r\n", 6)) {
                start_data_ = true;
            }
            else if (payload_size >= 6 && char_cmp((char*)payload, "QUIT\r\n", 6)) {
                quitted_ = true;
            }
        }
        else {
            client_buffer_->write(reinterpret_cast<char*>(payload), payload_size);
            if (
                    payload_size >= 5 &&
                    char_cmp((char*)payload+payload_size-5, "\r\n.\r\n", 5)
                    ) {
                LOG_DEBUG << string(client_buffer_->data_to_read(), client_buffer_->size());
                multipart_parse(*client_buffer_, stream_id_);
                client_buffer_->clear();
                start_data_ = false;
            }
        }
    }

    void Sniffer::on_server_payload(PayloadVector payload, size_t payload_size) {
        LOG_TRACE << "SMTP server data size :" << payload_size;
    }

    void Sniffer::on_connection_close() {
        LOG_DEBUG << stream_id_.to_string() << " " << "SMTP Connection Close" << std::endl;

//        auto file_vec = mail_process(std::string(
//                client_buffer_->data_to_read(),
//                client_buffer_->size()
//        ));
//
//        submit_files_and_delete(file_vec);


    }

    void Sniffer::on_connection_terminated(
            TerminationReason) {
        LOG_DEBUG << stream_id_.to_string() << " SMTP data connection terminated.";
    }

    Sniffer::Sniffer(const cs::base::StreamIdentifier& stream_id, int thread_id) : TCPSniffer(stream_id, thread_id) {

        client_buffer_ = new Buffer();
        start_data_ = false;
        quitted_ = false;

    }

    Sniffer::~Sniffer() {
        delete client_buffer_;
    }

}
