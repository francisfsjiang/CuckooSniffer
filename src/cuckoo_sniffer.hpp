#ifndef CUCKOOSNIFFER_CUCKOO_SNIFFER_HPP
#define CUCKOOSNIFFER_CUCKOO_SNIFFER_HPP

#include <boost/log/trivial.hpp>


#define LOG_TRACE       BOOST_LOG_SEV(cs::lg, boost::log::trivial::trace)
#define LOG_DEBUG       BOOST_LOG_SEV(cs::lg, boost::log::trivial::debug)
#define LOG_INFO        BOOST_LOG_SEV(cs::lg, boost::log::trivial::info)
#define LOG_WARNING     BOOST_LOG_SEV(cs::lg, boost::log::trivial::warning)
#define LOG_ERROR       BOOST_LOG_SEV(cs::lg, boost::log::trivial::error)
#define LOG_FATAL       BOOST_LOG_SEV(cs::lg, boost::log::trivial::fatal)


namespace cs {


extern boost::log::sources::severity_logger <boost::log::trivial::severity_level> lg;

void init_log();

void init_log_in_thread();


}

#endif //CUCKOOSNIFFER_CUCKOO_SNIFFER_HPP
