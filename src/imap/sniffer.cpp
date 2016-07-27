#include "imap/sniffer.hpp"

#include <util/function.hpp>
#include "sniffer_manager.hpp"
#include "imap/collected_data.hpp"
#include "imap/data_processor.hpp"

namespace cs { namespace imap {

void Sniffer::on_client_payload(const Tins::TCPIP::Stream &stream) {
    if (status_ != Status::NONE) {

        //TODO make this in thread
        DataProcessor processor;
        processor.process(*sniffer_data_);
        delete sniffer_data_;


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
            stream.client_payload().begin(),
            stream.client_payload().end()
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
			sniffer_data_ = new CollectedData();
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

void Sniffer::on_server_payload(const Tins::TCPIP::Stream &stream) {
    std::string data = std::string(
            stream.server_payload().begin(),
            stream.server_payload().end()
    );
    switch (status_) {
        case Status::NONE:
            break;
        case Status::MULTI:
//        std::cout << "server payload " << std::endl << data << std::endl;
            sniffer_data_->append(data);
            break;
        case Status::PART:
            sniffer_data_->append(data);
            break;
        default:
            return;
    }
}

void Sniffer::on_connection_close(const Tins::TCPIP::Stream &stream) {
    std::cout << "Connection Close" << std::endl;
    cs::SNIFFER_MANAGER.erase_sniffer(id_);
}

void Sniffer::on_connection_terminated(
        Tins::TCPIP::Stream &,
        Tins::TCPIP::StreamFollower::TerminationReason) {
    std::cout << "[+] On Connection terminated " << id_ << std::endl;
    cs::SNIFFER_MANAGER.erase_sniffer(id_);
}

Sniffer::Sniffer(Tins::TCPIP::Stream &stream) : TCPSniffer(stream) {
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

}}