#include "sniffer_manager.hpp"

#include "sniffer/smtp_sniffer.hpp"

SnifferManager& SNIFFER_MANAGER = SnifferManager::get_instance();

SnifferManager SnifferManager::instance;

SnifferManager& SnifferManager::get_instance() {
    return instance;
}

Sniffer* SnifferManager::get_sniffer(const u_int16_t& port) {
    switch (port) {
        case 25: //SMTP
            return new SMTPSniffer();
        default:
            return nullptr;
    }
}

SnifferManager::SnifferManager() {

}