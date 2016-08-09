#include "samba/collected_data.hpp"

#include "util/file.hpp"

namespace cs {
namespace samba {

CollectedData::CollectedData(
        cs::util::File* file) :
        cs::base::CollectedData(DataType::SAMBA) {
    file_ = file;
}

cs::util::File* CollectedData::get_data() const {
    return file_;
}
CollectedData::~CollectedData() {
    delete file_;
}

}
}