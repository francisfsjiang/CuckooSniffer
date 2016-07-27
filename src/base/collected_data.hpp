#ifndef CUCKOOSNIFFER_BASE_COLLECTED_DATA_HPP
#define CUCKOOSNIFFER_BASE_COLLECTED_DATA_HPP

namespace cs { namespace base {

class CollectedData {

public:

    enum DataType {
        SMTP,
        IMAP,
        FTP,
        HTTP,
        SAMBA
    };

    CollectedData() = delete;


    virtual ~CollectedData() {};

    inline virtual DataType get_data_type() {
        return data_type_;
    };

protected:

    DataType data_type_;

    CollectedData(DataType data_type) : data_type_(data_type) {}

};

}}

#endif //CUCKOOSNIFFER_SNIFFER_CONTENT_HPP
