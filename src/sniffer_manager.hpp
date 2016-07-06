#ifndef CUCKOOSNIFFER_SNIFFER_MANAGER_HPP
#define CUCKOOSNIFFER_SNIFFER_MANAGER_HPP

#include <i386/types.h>

class Sniffer;

class SMTPSniffer;

class SnifferManager{

public:
    static SnifferManager instance;

    static SnifferManager& get_instance();

    Sniffer* get_sniffer(const u_int16_t& port);

private:

    SnifferManager();

};

extern SnifferManager& SNIFFER_MANAGER;


#endif //CUCKOOSNIFFER_SNIFFER_MANAGER_HPP
