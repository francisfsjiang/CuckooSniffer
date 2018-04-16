#include "samba/collected_data.hpp"

#include "util/file.hpp"

namespace cs::samba {

    using namespace cs::base;

    CollectedData::CollectedData(
            cs::util::File* file) :
            cs::base::CollectedData(DataType::SMTP),
            file_(file)
    {
    }

}
