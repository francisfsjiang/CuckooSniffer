#ifndef CUCKOOSNIFFER_BASE_DATA_PROCESSOR_HPP
#define CUCKOOSNIFFER_BASE_DATA_PROCESSOR_HPP

#include <functional>
#include <vector>
#include <map>

namespace cs::util {
    class Buffer;
    class File;
}

namespace cs::base {

    class CollectedData;

    enum class DataType;

    typedef std::function<std::vector<cs::util::File*>(cs::base::CollectedData* collected_data)> ProcessorFunc;

    extern std::map<DataType, ProcessorFunc> ProcessorRouter;
}

#endif //CUCKOOCOLLECTED_DATA_PROCESSOR_HPP
