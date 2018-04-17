#include "imap/collected_data.hpp"

namespace cs::imap {

    using namespace cs::base;


    CollectedData::CollectedData(
            cs::util::Buffer* client_buffer) :
            cs::base::CollectedData(DataType::IMAP),
            client_buffer_(client_buffer)
    {
    }

}
