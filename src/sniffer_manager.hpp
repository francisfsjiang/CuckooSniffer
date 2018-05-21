#ifndef CUCKOOSNIFFER_SNIFFER_MANAGER_HPP
#define CUCKOOSNIFFER_SNIFFER_MANAGER_HPP

#include <string>
#include <map>
#include <thread>
#include <mutex>

namespace cs::base {
    class StreamIdentifier;
    class Sniffer;
}

namespace cs {

class SnifferManager {

public:
    static SnifferManager instance;

    static SnifferManager &get_instance();

    void append_sniffer(const cs::base::StreamIdentifier&, std::shared_ptr<cs::base::Sniffer>, int);

    std::pair<std::shared_ptr<cs::base::Sniffer>, int> get_sniffer_info(const cs::base::StreamIdentifier&);

    void erase_sniffer(const cs::base::StreamIdentifier&);

private:

    struct sniffer_container_cmp {
        bool operator()(const cs::base::StreamIdentifier& lhs, const cs::base::StreamIdentifier& rhs) const;
    };
    std::map<cs::base::StreamIdentifier, std::pair<std::shared_ptr<cs::base::Sniffer>, int>, sniffer_container_cmp> sniffer_container;

    std::mutex lock_;

    SnifferManager();

};

extern SnifferManager& SNIFFER_MANAGER;

}


#endif //CUCKOOSNIFFER_SNIFFER_MANAGER_HPP
