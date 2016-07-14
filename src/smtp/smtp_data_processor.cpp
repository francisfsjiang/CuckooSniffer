

#include "smtp/smtp_data_processor.hpp"
#include "smtp/smtp_sniffer_data.hpp"
#include "util/base64.hpp"
#include "util/file.hpp"
#include "util/mail_process.hpp"


int SMTPDataProcessor::process(const SnifferData& sniffer_data_arg) {

    const SMTPSnifferData& sniffer_data = (const SMTPSnifferData&)sniffer_data_arg;
    mail_process(sniffer_data.get_data());

    return 1;

}


SMTPDataProcessor::~SMTPDataProcessor() { }
