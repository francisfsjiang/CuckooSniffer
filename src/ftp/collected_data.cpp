#include "ftp/collected_data.hpp"

namespace cs::ftp {

    using namespace cs::base;

    CollectedData::CollectedData(
            cs::util::Buffer* server_buffer) :
            cs::base::CollectedData(DataType::FTP),
            server_buffer_(server_buffer)
    {
    }

}
