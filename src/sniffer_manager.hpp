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

    void append_sniffer(std::string, cs::base::Sniffer *);

    cs::base::Sniffer *get_sniffer(std::string);

    void erase_sniffer(std::string);

private:

    std::map<std::string, cs::base::Sniffer *> sniffer_container;

    SnifferManager();

};

extern SnifferManager& SNIFFER_MANAGER;

}


#endif //CUCKOOSNIFFER_SNIFFER_MANAGER_HPP
