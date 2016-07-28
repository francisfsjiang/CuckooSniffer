#include "base/data_processor.hpp"

#include "ftp/data_processor.hpp"
#include "http/data_processor.hpp"
#include "smtp/data_processor.hpp"
#include "imap/data_processor.hpp"
#include "samba/data_processor.hpp"

namespace cs {
namespace base {

DataProcessor* get_data_processor_by_data_type(const CollectedData::DataType& type) {
    switch (type) {
        case CollectedData::SMTP:
            return new cs::smtp::DataProcessor();
        case CollectedData::IMAP:
            return new cs::imap::DataProcessor();
        case CollectedData::FTP:
            return new cs::ftp::DataProcessor();
        case CollectedData::HTTP:
            return new cs::http::DataProcessor();
        case CollectedData::SAMBA:
            return new cs::samba::DataProcessor();
        default:
            return nullptr;
    }
}

}
}

