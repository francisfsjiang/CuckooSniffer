#include <strstream>
#include <iostream>
#include <map>
#include <regex>
#include <set>

#include "smtp_data_processor.hpp"
#include "smtp_sniffer_data.hpp"
#include "util/base64.hpp"
#include "util/file.hpp"


int SMTPDataProcessor::process(const SnifferData& sniffer_data_arg) {

    const SMTPSnifferData& sniffer_data = (const SMTPSnifferData&)sniffer_data_arg;
    const std::string& data = sniffer_data.get_data();
//    std::cout << "***************" << std::endl;
//    std::cout << data << std::endl;
//    std::cout << "***************" << std::endl;
    std::cout << "Start process SMTP" << std::endl;

    std::istrstream input_stream(data.c_str());

    std::multimap<std::string, std::string> record;

    //based on RFC5321

    static const std::set<std::string> target_file_type = {
            "application/octet-stream",
            "text/x-script.phyton"
    };

    static const std::regex         hello_command("(?:EHLO|HELO) (\\S+)");
    static const std::regex          auth_command("AUTH PLAIN (\\S+)");
    static const std::regex          mail_command("(?:MAIL|mail) FROM\\:(\\S+)");
    static const std::regex          rcpt_command("(?:RCPT|rcpt) TO\\:(\\S+)");
    static const std::regex          date_command("Date: (.+)");
    static const std::regex            ua_command("User-Agent: (.+)");
    static const std::regex      boundary_command("boundary=\"([\\w-]+)\"");
    static const std::regex  content_type_command("Content-Type: ([-/\\w]+)");
    static const std::regex          name_command("name=\"([-\\=\\+\?\\w\\.]+)\"");
    static const std::regex      encoding_command("Content-Transfer-Encoding: (\\w+)");

    bool have_boundary = false;
    bool file_section = false;
    std::string line, s, boundary;
    std::string file_mime_type, file_name, file_encoding;

    try {
        while (std::getline(input_stream, line)) {
            if ((s = match(line, hello_command)) != "") {
                record.insert(std::make_pair("domin", s));
            }
            else if ((s = match(line, auth_command)) != "") {
                record.insert(std::make_pair("auth_info", base64_decode(s)));
            }
            else if ((s = match(line, mail_command)) != "") {
                record.insert(std::make_pair("sender_address", s));
            }
            else if ((s = match(line, rcpt_command)) != "") {
                record.insert(std::make_pair("receiver_address", s));
            }
            else if ((s = match(line, date_command)) != "") {
                record.insert(std::make_pair("date", s));
            }
            else if ((s = match(line, ua_command)) != "") {
                record.insert(std::make_pair("user_agent", s));
            }
            else if ((s = match(line, boundary_command)) != "") {
                boundary = s;
                have_boundary = true;
                std::cout << "get boundary," << s << std::endl;
                break;
            }
        }
        std::getline(input_stream, line);
        for(const auto& iter: record) {
            std::cout << iter.first << ":" << iter.second << std::endl;
        }
        // after find boundary
        while (true) {
            if (is_boundary(boundary, line)) {
                std::getline(input_stream, line);
                if (line == "\r") std::getline(input_stream, line);
                if ((s = match(line, content_type_command)) != "") {
                    if (target_file_type.find(s) != target_file_type.end()) {
                        file_section = true;
                    }
                }
                else {
                    file_section = false;
                }
                if (file_section) { //read and record file section

                    file_mime_type = s;
                    std::getline(input_stream, line);
                    file_name = match(line, name_command);
                    std::getline(input_stream, line);
                    file_encoding = match(line, encoding_command);

                    File* f = new File(
                            file_name,
                            file_mime_type,
                            file_encoding
                    );

                    do {
                        std::getline(input_stream, line);
                    } while (line != "\r");

                    if (file_encoding == "base64") {
                        while (true) {
//                            std::getline(input_stream, line);
                            input_stream >> line;
                            if (is_boundary(boundary, line) || line == "") {
                                break;
                            }
                            else {
                                f -> add_content(line);
                            }
                        }
                        std::cout << f ->get_name() << std::endl;
                        std::cout << f ->get_mime_type() << std::endl;
                        std::cout << f -> get_encoding() << std::endl;
                        std::cout << f -> get_content() << std::endl;

                        delete f; //TODO delete this
                    }
                    else {

                    }

                }
                else { //skip this non-file section
                    do {
                        std::getline(input_stream, line);
                    } while (!is_boundary(boundary, line));
                    continue;
                }
            }
            else { //skip all the non-boundary line
                std::getline(input_stream, line);
                continue;
            }
        }
    }
    catch (std::exception e){

    }

    return 1;

}

std::string SMTPDataProcessor::match(const std::string& s, const std::regex& re) {
    std::smatch match;
    if (std::regex_search(s, match, re) && match.size() > 1) {
        return match.str(1);
    }
    else {
        return "";
    }
}

bool SMTPDataProcessor::is_boundary(const std::string& boundary, const std::string& line) {
    return line.find(boundary) != std::string::npos;
}


SMTPDataProcessor::~SMTPDataProcessor() { }