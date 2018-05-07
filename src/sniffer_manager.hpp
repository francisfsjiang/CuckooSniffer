#ifndef CUCKOOSNIFFER_SNIFFER_MANAGER_HPP
#define CUCKOOSNIFFER_SNIFFER_MANAGER_HPP

#include <string>
#include <map>

namespace cs {

namespace base {

class Sniffer;

}

class SnifferManager {

public:
    static SnifferManager instance;

    static SnifferManager &get_instance();

    void append_sniffer(std::string, std::shared_ptr<cs::base::Sniffer>, int);

    std::pair<std::shared_ptr<cs::base::Sniffer>, int> get_sniffer_info(std::string);

    void erase_sniffer(std::string);

private:

    std::map<std::string, std::pair<std::shared_ptr<cs::base::Sniffer>, int>> sniffer_container;

    SnifferManager();

};

extern SnifferManager& SNIFFER_MANAGER;

}


#endif //CUCKOOSNIFFER_SNIFFER_MANAGER_HPP
