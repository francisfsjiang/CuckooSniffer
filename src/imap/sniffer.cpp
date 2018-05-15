#include "imap/sniffer.hpp"

#include <iostream>

#include "cuckoo_sniffer.hpp"
#include "util/function.hpp"
#include "util/buffer.hpp"
#include "sniffer_manager.hpp"
#include "imap/data_processor.hpp"
#include "imap/collected_data.hpp"

namespace cs::imap {
    using namespace cs::util;
    using namespace cs::base;

    void Sniffer::on_client_payload(const cs::base::PayloadType& payload) {
        if (status_ != Status::NONE) {

            cs::DATA_QUEUE.enqueue(new cs::imap::CollectedData(sniffer_data_));

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
                payload.begin(),
                payload.end()
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

    void Sniffer::on_server_payload(const cs::base::PayloadType& payload) {
        switch (status_) {
            case Status::NONE:
                break;
            case Status::MULTI:
//        std::cout << "server payload " << std::endl << data << std::endl;
                sniffer_data_->write(
                        reinterpret_cast<const char*>(payload.data()),
                        payload.size()
                );
                break;
            case Status::PART:
                sniffer_data_->write(
                        reinterpret_cast<const char*>(payload.data()),
                        payload.size()
                );
                break;
            default:
                return;
        }
    }

    void Sniffer::on_connection_close() {
        std::cout << "Connection Close" << std::endl;
        cs::SNIFFER_MANAGER.erase_sniffer(id_);
    }

    void Sniffer::on_connection_terminated(
            Tins::TCPIP::Stream &,
            Tins::TCPIP::StreamFollower::TerminationReason) {
        LOG_DEBUG << id_ << " IMAP connection terminated.";
        cs::SNIFFER_MANAGER.erase_sniffer(id_);
    }

    Sniffer::Sniffer(const std::string& stream_id) : TCPSniffer(stream_id) {
        std::cout << "get 143 connection" << std::endl;
        stream.auto_cleanup_client_data(true);
        stream.auto_cleanup_server_data(true);
        stream.client_data_callback(
                [this](const Tins::TCPIP::Stream &tcp_stream) {
                    this->on_client_payload(tcp_stream);
                }
        );

        stream.server_data_callback(
                [this](const Tins::TCPIP::Stream &tcp_stream) {
                    this->on_server_payload(tcp_stream);
                }
        );

        stream.stream_closed_callback(
                [this](const Tins::TCPIP::Stream &tcp_stream) {
                    this->on_connection_close(tcp_stream);
                }
        );

    }

    Sniffer::~Sniffer() {
    }

}