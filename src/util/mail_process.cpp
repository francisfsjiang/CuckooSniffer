#include "util/mail_process.hpp"

#include <iostream>
#include <sstream>
#include <map>
#include <regex>
#include <set>

#include "util/file.hpp"

namespace cs::util {

std::string match(const std::string &s, const std::regex &re) {
    std::smatch match;
    if (std::regex_search(s, match, re) && match.size() > 1) {
        return match.str(1);
    }
    else {
        return "";
    }
}

bool is_boundary(const std::string &line, const std::string &boundary) {
    return line.find(boundary) != std::string::npos;
}

std::vector<File *> mail_process(const std::string &data) {

    std::istringstream input_stream(data.c_str());

    std::multimap<std::string, std::string> record;

    //based on RFC5321


    static const std::set<std::string> target_file_type = {
            "application/octet-stream",
            "application/zip",
            "application/x-msdownload",
            "text/x-script.phyton"
    };

    static const std::regex hello_command("(?:EHLO|HELO) (\\S+)");
    static const std::regex auth_command("AUTH PLAIN (\\S+)");
    static const std::regex mail_command("(?:MAIL|mail) FROM\\:\\s*(\\S+)");
    static const std::regex rcpt_command("(?:RCPT|rcpt) TO\\:\\s*(\\S+)");
    static const std::regex date_command("Date: (.+)");
    static const std::regex ua_command("User-Agent: (.+)");
    static const std::regex boundary_command("boundary=\"([\\w_=\\.-]+)\"");
    static const std::regex content_type_command("Content-[Tt]ype:\\s*([-/\\w]+)");
    static const std::regex name_command("name=\"([-\\=\\+\?\\w\\.]+)\"");
    static const std::regex encoding_command("Content-[Tt]ransfer-[Ee]ncoding: (\\w+)");
    static const std::regex quit_command("(QUIT)");

    bool have_boundary = false;
    bool file_section = false;
    std::string line, s, boundary;
    std::string file_mime_type, file_name, file_encoding;

    std::vector<File *> file_vec;

    try {

        //read email headers
        while (std::getline(input_stream, line)) {
            if ((s = match(line, hello_command)) != "") {
                record.insert(std::make_pair("domin", s));
            }
            if ((s = match(line, auth_command)) != "") {
                record.insert(std::make_pair("auth_info", s));
            }
            if ((s = match(line, mail_command)) != "") {
                record.insert(std::make_pair("sender_address", s));
            }
            if ((s = match(line, rcpt_command)) != "") {
                record.insert(std::make_pair("receiver_address", s));
            }
            if ((s = match(line, date_command)) != "") {
                record.insert(std::make_pair("date", s));
            }
            if ((s = match(line, ua_command)) != "") {
                record.insert(std::make_pair("user_agent", s));
            }
            if ((s = match(line, boundary_command)) != "") {
                boundary = s;
                have_boundary = true;
//                std::cout << "get boundary  " << s << std::endl;
                break;
            }
        }
        for (const auto &iter: record) {
            std::cout << iter.first << ":" << iter.second << std::endl;
        }

        //find the first boundary
        while (std::getline(input_stream, line)) {
            if (is_boundary(line, boundary)) {
                break;
            }
        }
        while (true) {
            //init
            file_mime_type = file_name = file_encoding = "";

            //now line is boundary
            while (std::getline(input_stream, line)) {
                if (line == "" || line == "\r") {
                    break;
                }
                if (file_mime_type == "")
                    file_mime_type = match(line, content_type_command);
                if (file_name == "")
                    file_name = match(line, name_command);
                if (file_encoding == "")
                    file_encoding = match(line, encoding_command);
            }

            if (file_mime_type == "" && file_name == "" && file_encoding == "") {
                break;
            }

            if (target_file_type.find(file_mime_type) == target_file_type.end()) {
                //not in file section, go to next boundary
                while (std::getline(input_stream, line)) {
                    if (is_boundary(line, boundary)) {
                        break;
                    }
                }
                continue;
            }

            File *f = new File();
            f -> set_name(file_name);
            f -> set_mime_type(file_mime_type);
            f -> set_mime_type(file_encoding);


            if (file_encoding == "base64") {
                while (true) {
//                            std::getline(input_stream, line);
                    input_stream >> line;
                    if (is_boundary(line, boundary)) {
                        char ch;
                        do {
                            ch = '\0';
                            input_stream.get(ch);
                        } while (ch == '\n' || ch == '\r');
                        input_stream.putback(ch);
                        break;
                    }
                    if (line == "" || line == "\r") {
                        continue;
                    }
                    else {
                        f->write(line.c_str(), line.size());
                    }
                }
                std::cout << f->get_name() << std::endl;
                std::cout << f->get_mime_type() << std::endl;
                std::cout << "size: " << f->get_size() << std::endl << std::endl;
                file_vec.push_back(f);
            }
            else {

            }

        }
    }
    catch (std::exception e) {
    }
    return file_vec;
}

}
