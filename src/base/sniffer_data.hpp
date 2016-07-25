#ifndef CUCKOOSNIFFER_SNIFFER_DATA_HPP
#define CUCKOOSNIFFER_SNIFFER_DATA_HPP

namespace cs::base {

class SnifferData {

public:

    enum DataType {
        SMTP,
        IMAP,
        FTP,
        HTTP,
        SAMBA
    };

    SnifferData() = delete;


    virtual ~SnifferData() {};

    inline virtual DataType get_data_type() {
        return data_type_;
    };

protected:

    DataType data_type_;

    SnifferData(DataType data_type) : data_type_(data_type) {}

};

}

#endif //CUCKOOSNIFFER_SNIFFER_CONTENT_HPP
