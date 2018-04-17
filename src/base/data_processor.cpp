#include "base/data_processor.hpp"

#include "base/collected_data.hpp"
#include "ftp/data_processor.hpp"
#include "http/data_processor.hpp"
#include "smtp/data_processor.hpp"
#include "imap/data_processor.hpp"
#include "samba/data_processor.hpp"

namespace cs::base {

    std::map<DataType, ProcessorFunc> ProcessorRouter = {
            {DataType::HTTP, cs::http::processor},
            {DataType::FTP,  cs::ftp::processor},
            {DataType::IMAP, cs::imap::processor},
            {DataType::SMTP, cs::smtp::processor},
            {DataType::SAMBA,cs::samba::processor}
    };

}

