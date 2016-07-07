#ifndef CUCKOOSNIFFER_SNIFFER_DATA_HPP
#define CUCKOOSNIFFER_SNIFFER_DATA_HPP


class SnifferData {

public:

    enum DataType {
        SMTP,
        IMAP
    };

    virtual ~SnifferData() {};

    inline virtual DataType get_data_type() {
        return data_type_;
    };

protected:

    DataType data_type_;


};

#endif //CUCKOOSNIFFER_SNIFFER_CONTENT_HPP
