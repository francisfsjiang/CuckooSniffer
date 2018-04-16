#include "http/collected_data.hpp"

namespace cs::http {

    using namespace cs::base;

    CollectedData::CollectedData(
            cs::util::Buffer* client_buffer,
            cs::util::Buffer* server_buffer) :
            cs::base::CollectedData(DataType::SMTP),
            client_buffer_(client_buffer),
            server_buffer_(server_buffer)
    {
    }

}
