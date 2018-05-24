#include "imap/sniffer.hpp"

#include <iostream>
#include <regex>

#include "cuckoo_sniffer.hpp"
#include "util/function.hpp"
#include "util/buffer.hpp"
#include "sniffer_manager.hpp"
#include "util/mail_process.hpp"

namespace cs::imap {

    using namespace std;
    using namespace cs::util;
    using namespace cs::base;

    std::vector<cs::util::File*> process(Buffer* buffer) {

        std::string data = std::string(buffer->data_to_read(), buffer->size());

        std::cout << "stat process imap data" << std::endl;
        //std::cout << data << std::endl << std::endl;

        static const std::regex departer("\\* \\d* FETCH \\(UID \\d* (?:RFC822.SIZE \\d* )?BODY\\[\\] \\{\\d*\\}([\\s^\\S]*?)\n\\)\r\n");
        std::smatch match;
        try
        {
            while (regex_search(data, match, departer))
            {
                util::mail_process(match.str(1));
                data = match.suffix();
            }

        }
        catch (const std::exception&)
        {
            std::cerr << "Regex error" << std::endl;
        }

    }


    void Sniffer::on_client_payload(PayloadVector payload, size_t payload_size) {
        if (status_ != Status::NONE) {

            auto file_vec = process(sniffer_data_);
            submit_files_and_delete(file_vec);


            status_ = Status::NONE;
            sniffer_data_ = nullptr;
        }

        static const std::regex multi_email(
                "[\\w]* UID (?:fetch)|(?:FETCH) ([\\d,:]*) \\(UID (?:RFC822.SIZE )?BODY.PEEK\\[\\]\\)");
        static const std::regex part_email(
                "[\\w]* UID (?:fetch)|(?:FETCH) ([\\d,:]*) \\(UID (?:RFC822.SIZE )?BODY.PEEK\\[\\]\\<([\\d,]*)\\>\\)");

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
    }

    void Sniffer::on_connection_terminated(
            TerminationReason) {
        LOG_DEBUG << stream_id_.to_string() << " IMAP connection terminated.";
    }

    Sniffer::Sniffer(const cs::base::StreamIdentifier& stream_id, int thread_id) : TCPSniffer(stream_id, thread_id) {
        std::cout << "get 143 connection" << std::endl;
    }

    Sniffer::~Sniffer() {
    }

}