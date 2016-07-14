#ifndef CUCKOOSNIFFER_SNIFFER_MANAGER_HPP
#define CUCKOOSNIFFER_SNIFFER_MANAGER_HPP

#include <tins/pdu.h>
#include "sniffer.hpp"

class Sniffer;

class SnifferManager{

public:
    static SnifferManager instance;

    static SnifferManager& get_instance();

    void append_sniffer(std::string, Sniffer*);
    Sniffer* get_sniffer(std::string);
    void erase_sniffer(std::string);

private:

    std::map<std::string, Sniffer*> sniffer_container;

    SnifferManager();

};

extern SnifferManager& SNIFFER_MANAGER;


#endif //CUCKOOSNIFFER_SNIFFER_MANAGER_HPP
