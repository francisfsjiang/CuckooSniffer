#ifndef CUCKOOSNIFFER_BASE_COLLECTED_DATA_HPP
#define CUCKOOSNIFFER_BASE_COLLECTED_DATA_HPP

namespace cs::util {
    class Buffer;
}

namespace cs::base {

    enum class DataType: int {
        FTP = 0,
        HTTP = 1,
        SAMBA = 2,
        IMAP = 3,
        SMTP = 4,
    };
class CollectedData {

public:

    CollectedData() = delete;

    virtual ~CollectedData() {};

    inline virtual DataType get_data_type() {
        return data_type_;
    };

protected:

    DataType data_type_;

    CollectedData(DataType data_type) : data_type_(data_type) {}

};

}

#endif //CUCKOOSNIFFER_SNIFFER_CONTENT_HPP
