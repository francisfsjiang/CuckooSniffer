#include "pcpp_capturer.hpp"


#include <arpa/inet.h>
#include <sys/un.h>
#include <netinet/in.h>

#include "IpUtils.h"


#include "cuckoo_sniffer.hpp"


namespace cs {

    using namespace std;
    using namespace pcpp;

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

    PcpppCapturer::PcpppCapturer(string iface, string filter){
        iface_ = iface;
        filter_ = filter;
        stopped_ = false;

        dev_ = PcapLiveDeviceList::getInstance().getPcapLiveDeviceByName(iface_);
        if (dev_ == NULL) {
            LOG_ERROR << "Couldn't find interface by provided name";
            exit(1);
        }
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

        dev_->startCapture(on_packet_arrives, &tcp_reassembly_);

        ApplicationEventHandler::getInstance().onApplicationInterrupted(on_app_interrupted, &stopped_);

        while(!stopped_)
            PCAP_SLEEP(1);

        dev_->stopCapture();
        dev_->close();

        tcp_reassembly_.closeAllConnections();

        LOG_INFO << "Done! processed %d connections " << (int)tcp_reassembly_.getConnectionInformation().size();
    }

    void PcpppCapturer::on_packet_arrives(RawPacket* packet, PcapLiveDevice* dev, void* tcpReassemblyCookie)
    {
        std::string data((char*)packet->getRawData(), packet->getRawDataLen());
        LOG_TRACE << "On packet arrive." << data;

        TcpReassembly* tcpReassembly = (TcpReassembly*)tcpReassemblyCookie;
        tcpReassembly->reassemblePacket(packet);
    }

    void PcpppCapturer::tcp_msg_ready_callback(int sideIndex, TcpStreamData tcpData, void* userCookie)
    {
        LOG_TRACE << "Msg ready callback";
        // extract the connection manager from the user cookie
//        TcpReassemblyConnMgr* connMgr = (TcpReassemblyConnMgr*)userCookie;
//
//        // check if this flow already appears in the connection manager. If not add it
//        TcpReassemblyConnMgrIter iter = connMgr->find(tcpData.getConnectionData().flowKey);
//        if (iter == connMgr->end())
//        {
//            connMgr->insert(std::make_pair(tcpData.getConnectionData().flowKey, TcpReassemblyData()));
//            iter = connMgr->find(tcpData.getConnectionData().flowKey);
//        }

        LOG_TRACE << "Site: " << sideIndex << " , size " << tcpData.getDataLength();
    }

    void PcpppCapturer::tcp_connection_start_callback(ConnectionData connectionData, void* userCookie)
    {
        LOG_TRACE << "Conn start callback" ;
        // get a pointer to the connection manager
//        TcpReassemblyConnMgr* connMgr = (TcpReassemblyConnMgr*)userCookie;

//        // look for the connection in the connection manager
//        TcpReassemblyConnMgrIter iter = connMgr->find(connectionData.flowKey);
//
//        // assuming it's a new connection
//        if (iter == connMgr->end())
//        {
//            // add it to the connection manager
//            connMgr->insert(std::make_pair(connectionData.flowKey, TcpReassemblyData()));
//        }
    }

    void PcpppCapturer::tcp_connection_end_callback(ConnectionData connectionData, TcpReassembly::ConnectionEndReason reason, void* userCookie)
    {
        LOG_TRACE << "Conn end callback";
        // get a pointer to the connection manager
//        TcpReassemblyConnMgr* connMgr = (TcpReassemblyConnMgr*)userCookie;
//
//        // find the connection in the connection manager by the flow key
//        TcpReassemblyConnMgrIter iter = connMgr->find(connectionData.flowKey);
//
//        // connection wasn't found - shouldn't get here
//        if (iter == connMgr->end())
//            return;
//
//        // remove the connection from the connection manager
//        connMgr->erase(iter);
    }

    void PcpppCapturer::on_app_interrupted(void *cookie)
    {
        bool* shouldStop = (bool*)cookie;
        *shouldStop = true;
    }
}
