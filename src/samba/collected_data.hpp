#ifndef CUCKOOSNIFFER_SAMBA_COLLECTED_DATA_HPP
#define CUCKOOSNIFFER_SAMBA_COLLECTED_DATA_HPP


#include <string>
#include "base/collected_data.hpp"

namespace cs {

namespace util {

class File;

}

namespace samba {

class CollectedData: public cs::base::CollectedData {

public:

    CollectedData(cs::util::File*);

    cs::util::File* get_data() const;

    virtual ~CollectedData();

private:

    cs::util::File* file_;

};

}
}


#endif //CUCKOOSNIFFER_SAMBA_COLLECTED_DATA_HPP
