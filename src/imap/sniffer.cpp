#include "imap/sniffer.hpp"

#include <iostream>
#include <regex>

#include <boost/algorithm/string.hpp>
#include "mime/Mime.h"

#include "cuckoo_sniffer.hpp"
#include "util/function.hpp"
#include "util/buffer.hpp"
#include "sniffer_manager.hpp"
#include "util/mail_process.hpp"
#include "util/file.hpp"

namespace cs::imap {

    using namespace std;
    using namespace cs::util;
    using namespace cs::base;


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

//        LOG_TRACE << std::string(buffer.data_to_read(), buffer.size());

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

    void process(Buffer* buffer, const StreamIdentifier& stream_id) {

        std::string data = std::string(buffer->data_to_read(), buffer->size());

        std::cout << "stat process imap data" << std::endl;
//        std::cout << data << std::endl << std::endl;

        int start_pos = buffer->get_reader_pos();
        for(;start_pos+1 < buffer->get_writer_pos(); start_pos++) {
            if (buffer->data()[start_pos] == '\r' && buffer->data()[start_pos+1] == '\n') {
                buffer->set_reader_pos(start_pos+2);
                break;
            }
        }
        if (buffer->size() == 0)
            return;
        buffer->shrink();

        multipart_parse(*buffer, stream_id);

    }


    void Sniffer::on_client_payload(PayloadVector payload, size_t payload_size) {
//        auto data= std::string(
//                reinterpret_cast<char*>(payload),
//                payload_size
//        );
//        LOG_TRACE << "Client data: " << data;
        if (status_ != Status::NONE) {

            process(sniffer_data_, stream_id_);
            delete sniffer_data_;

            status_ = Status::NONE;
            sniffer_data_ = nullptr;
        }

        static const std::regex multi_email(
                "[\\w]* UID (?:fetch)|(?:FETCH) ([\\d,:]*) \\((?:uid)|(?:UID) (?:RFC822.SIZE )?(?:body.peek)|(?:BODY.PEEK)\\[\\]\\)");
        static const std::regex part_email(
                "[\\w]* UID (?:fetch)|(?:FETCH) ([\\d,:]*) \\((?:uid)|(?:UID) (?:RFC822.SIZE )?(?:body.peek)|(?:BODY.PEEK)\\[\\]\\<([\\d,]*)\\>\\)");

        std::string result;
        std::smatch match;
        std::string command, caught_str;

        command = std::string(
                reinterpret_cast<char*>(payload),
                payload_size
        );
        try
        {

            if (std::regex_search(command, match, multi_email) && match.size() > 1) {
                caught_str = match.str(1);
                LOG_TRACE << command;
                std::cout << "get multi email " << caught_str << std::endl;
                for (const auto& iter : util::split_str(caught_str, ",")) {
                    if (iter.find(":") != std::string::npos) {
                        const auto& vec = util::split_str(iter, ":");
                        std::cout << vec[0] << "   " << vec[1] << std::endl;
                    }
                    else {
                        std::cout << iter << std::endl;
                    }
                }
                status_ = Status::MULTI;
                sniffer_data_ = new Buffer();
            }
            else if (std::regex_search(command, match, part_email) && match.size() > 1) {
                caught_str = match.str(1);
                std::cout << "get part email " << caught_str << std::endl;
                const auto& vec = util::split_str(caught_str, ",");
                std::cout << vec[0] << " - " << vec[1] << std::endl;

                // status_ = Status::PART;
                // sniffer_data_ = new IMAPCollectedData();
            }
        }
        catch (const std::exception&)
        {
            std::cerr << "Regex error" << std::endl;
        }

    }


    void Sniffer::on_server_payload(PayloadVector payload, size_t payload_size) {
//        auto data= std::string(
//                reinterpret_cast<char*>(payload),
//                payload_size
//        );
//        LOG_TRACE << "Server data: " << data;

        switch (status_) {
            case Status::NONE:
                break;
            case Status::MULTI:
//        std::cout << "server payload " << std::endl << data << std::endl;
                sniffer_data_->write(
                        reinterpret_cast<const char*>(payload),
                        payload_size
                );
                break;
            case Status::PART:
                sniffer_data_->write(
                        reinterpret_cast<const char*>(payload),
                        payload_size
                );
                break;
            default:
                return;
        }
    }

    void Sniffer::on_connection_close() {
        std::cout << "Connection Close" << std::endl;
        if (sniffer_data_!= nullptr) {
            process(sniffer_data_, stream_id_);
            delete sniffer_data_;
            sniffer_data_ = nullptr;
        }
    }

    void Sniffer::on_connection_terminated(
            TerminationReason) {
        LOG_DEBUG << stream_id_.to_string() << " IMAP connection terminated.";
    }

    Sniffer::Sniffer(const cs::base::StreamIdentifier& stream_id, int thread_id) : TCPSniffer(stream_id, thread_id) {
        std::cout << "get 143 connection" << std::endl;
    }

    Sniffer::~Sniffer() {
        if (sniffer_data_!= nullptr) {
            delete sniffer_data_;
        }
    }

}