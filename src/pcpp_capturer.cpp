#include "pcpp_capturer.hpp"


#include <arpa/inet.h>
#include <sys/un.h>
#include <netinet/in.h>

#include "IpUtils.h"

#include "sniffer_manager.hpp"
#include "cuckoo_sniffer.hpp"
#include "smtp/sniffer.hpp"
#include "imap/sniffer.hpp"
#include "ftp/data_sniffer.hpp"
#include "ftp/command_sniffer.hpp"
#include "samba/sniffer.hpp"
#include "http/sniffer.hpp"
#include "base/sniffer.hpp"
#include "threads/thread.hpp"
#include "threads/data_queue.hpp"


namespace cs {

    using namespace std;
    using namespace pcpp;
    using namespace cs::base;
    using namespace cs::threads;

    void PcpppCapturer::generate_ignore_list(std::set<std::string>& ip_list)
    {
        const std::vector<PcapLiveDevice*>& devList = PcapLiveDeviceList::getInstance().getPcapLiveDevicesList();

        char buffer[INET6_ADDRSTRLEN];

        for (std::vector<PcapLiveDevice*>::const_iterator iter = devList.begin(); iter != devList.end(); iter++)
        {
            auto addresses = (*iter)->getAddresses();
            for(std::vector<pcap_addr_t>::iterator addrIter = addresses.begin(); addrIter != addresses.end(); addrIter++)
            {
                in_addr* addr4 = sockaddr2in_addr(addrIter->addr);
                if (addr4 != NULL)
                {
                    inet_ntop(AF_INET, addr4, buffer, sizeof(buffer));
                    ip_list.insert(string(buffer));
                    continue;
                }
                in6_addr* addr6 = sockaddr2in6_addr(addrIter->addr);
                if (addr6 != NULL)
                {
                    inet_ntop(AF_INET6, addr6, buffer, sizeof(buffer));
                    ip_list.insert(string(buffer));
                    continue;
                }
            }
        }
    }

    PcpppCapturer::PcpppCapturer(string iface, string filter, std::set<std::string> ignored_ip_address){
        iface_ = iface;
        filter_ = filter;
        stopped_ = false;
        current_thread_id_ = 0;
        sniffer_mannger_ = new SnifferManager();
        ignored_ip_address_ = ignored_ip_address;
        tcp_reassembly_ = new pcpp::TcpReassembly(
                tcp_msg_ready_callback,
                this,
                tcp_connection_start_callback,
                tcp_connection_end_callback
        );

        dev_ = PcapLiveDeviceList::getInstance().getPcapLiveDeviceByName(iface_);
        if (dev_ == NULL) {
            LOG_ERROR << "Couldn't find interface by provided name";
            exit(1);
        }
        LOG_TRACE << "Capture constructed, " << this;
    }

    PcpppCapturer::~PcpppCapturer() {
        LOG_TRACE << "Capture deconstructed, " << this;
    }

    int PcpppCapturer::get_thread_id() {
        int id = current_thread_id_++;
        if(current_thread_id_ >= cs::threads::THREADS_NUM) {
            current_thread_id_ = 0;
        }
        return id;
    }

    void PcpppCapturer::start() {
        if (!dev_->open()) {
            LOG_ERROR << "Cannot open interface";
            exit(1);
        }

        if (filter_!= "")
        {
            if (!dev_->setFilter(filter_)) {
                LOG_ERROR << "Cannot set BPF filter to interface";
                exit(1);
            }
        }

        LOG_INFO << "Starting packet capture on " << dev_->getIPv4Address().toString().c_str();

        dev_->startCapture(on_packet_arrives, this);

        ApplicationEventHandler::getInstance().onApplicationInterrupted(on_app_interrupted, &stopped_);

        while(!stopped_)
            PCAP_SLEEP(1);

        dev_->stopCapture();
        dev_->close();

        tcp_reassembly_->closeAllConnections();

        LOG_INFO << "Done! processed %d connections " << (int)tcp_reassembly_->getConnectionInformation().size();
    }

    void PcpppCapturer::on_packet_arrives(RawPacket* packet, PcapLiveDevice* dev, void* userCookie)
    {
        std::string data((char*)packet->getRawData(), packet->getRawDataLen());
//        LOG_TRACE << "On packet arrive." << data;

        auto capture = reinterpret_cast<PcpppCapturer*>(userCookie);

        capture->tcp_reassembly_->reassemblePacket(packet);

        auto conn_mgr = capture->sniffer_mannger_;
        conn_mgr->handle_time_out();
    }
//    int tsize = 0;
    void PcpppCapturer::tcp_msg_ready_callback(int sideIndex, TcpStreamData tcpData, void* userCookie)
    {
//        LOG_TRACE << "Msg ready callback";
        auto capture = reinterpret_cast<PcpppCapturer*>(userCookie);
        auto conn_mgr = capture->sniffer_mannger_;
        auto flow_key = tcpData.getConnectionData().flowKey;
        auto iter = conn_mgr->find(flow_key);
        if (!iter) {
//            LOG_ERROR << "No sniffer find. " << flow_key;
            return;
        }

//        LOG_TRACE << "Site: " << sideIndex << " , size " << tcpData.getDataLength();
//        tsize+=tcpData.getDataLength();
//        LOG_TRACE << "total_size: " << tsize;
        auto stream_data = new uint8_t[tcpData.getDataLength()];
        memcpy(stream_data, tcpData.getData(), tcpData.getDataLength());
//
        if(sideIndex == 0) {
            iter->data_callback(stream_data, tcpData.getDataLength(), DataType::CLIENT_PAYLOAD);
        }
        else {
            iter->data_callback(stream_data, tcpData.getDataLength(), DataType::SERVER_PAYLOAD);
        }
    }

    void PcpppCapturer::tcp_connection_start_callback(ConnectionData connectionData, void* userCookie)
    {
        auto flow_key = connectionData.flowKey;
//        LOG_TRACE << "Conn start callback. " << flow_key;

        auto capture = reinterpret_cast<PcpppCapturer*>(userCookie);
        string src_ip = connectionData.srcIP->toString();
        string dst_ip = connectionData.dstIP->toString();
        // get a pointer to the connection manager
        StreamIdentifier stream_id(
                src_ip,
                connectionData.srcPort,
                dst_ip,
                connectionData.dstPort,
                flow_key
        );


        cs::base::TCPSniffer* tcp_sniffer = nullptr;

        switch (stream_id.dst_port) {
            case 25:        //SMTP
                tcp_sniffer = new cs::smtp::Sniffer(stream_id, capture->get_thread_id());
                break;
            case 143:       //IMAP
                tcp_sniffer = new cs::imap::Sniffer(stream_id, capture->get_thread_id());
                break;
            case 21:        //FTP
                tcp_sniffer = new cs::ftp::CommandSniffer(stream_id, -1);
                break;
            case 80:        //HTTP
                if (
                        capture->ignored_ip_address_.find(src_ip) != capture->ignored_ip_address_.end() ||
                        capture->ignored_ip_address_.find(dst_ip) != capture->ignored_ip_address_.end()
                        ) {
                    LOG_INFO << "Ingoring stream " << flow_key;
                    return;
                }
                tcp_sniffer = new cs::http::Sniffer(stream_id, capture->get_thread_id());
                break;
            case 445:       //SAMBA
                tcp_sniffer = new cs::samba::Sniffer(stream_id, capture->get_thread_id());
                break;
            default:
                auto& ftp_data_connection = cs::ftp::CommandSniffer::get_data_connection_pool();
                auto iter = ftp_data_connection.find(make_pair(stream_id.dst_addr, stream_id.dst_port));

                if (iter != ftp_data_connection.end()) {
                    tcp_sniffer = new cs::ftp::DataSniffer(stream_id, capture->get_thread_id(), iter->second.first, iter->second.second);
                    ftp_data_connection.erase(iter);
                }
                break;
        }
        if (!tcp_sniffer) {
            return ;
        }

        LOG_TRACE << stream_id.to_string() << " Get tcp stream." ;
        std::shared_ptr<cs::base::TCPSniffer> sniffer_ptr = std::shared_ptr<cs::base::TCPSniffer>(tcp_sniffer);


        auto conn_mgr = capture->sniffer_mannger_;

        auto iter = conn_mgr->find(flow_key);
        if (!iter) {
            conn_mgr->insert(flow_key, sniffer_ptr);
        }
        LOG_TRACE << "Conn start end. " << flow_key;
    }

    void PcpppCapturer::tcp_connection_end_callback(ConnectionData connectionData, TcpReassembly::ConnectionEndReason reason, void* userCookie)
    {
        auto flow_key = connectionData.flowKey;
        LOG_TRACE << "Conn end callback, " << flow_key;
        auto capture = reinterpret_cast<PcpppCapturer*>(userCookie);
        auto conn_mgr = capture->sniffer_mannger_;

        conn_mgr->close_and_erase(flow_key);
    }

    void PcpppCapturer::on_app_interrupted(void *cookie)
    {
        bool* shouldStop = (bool*)cookie;
        *shouldStop = true;
    }
}
