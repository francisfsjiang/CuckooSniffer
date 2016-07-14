#include <regex>
#include <iostream>
#include <string>
#include <strstream>
#include <map>
#include <fstream>
#include <set>

#include "util/function.hpp"
#include "util/base64.hpp"
#include "util/file.hpp"

std::string match(const std::string& s, const std::regex& re) {
    std::smatch match;
    if (std::regex_search(s, match, re) && match.size() > 1) {
        return match.str(1);
    }
    else {
        return "";
    }
}

bool is_boundary(const std::string& line, const std::string& boundary) {
    return line.find(boundary) != std::string::npos;
}

void test() {
    std::ifstream ifs("tb_smtp.log");
    std::string data( (std::istreambuf_iterator<char>(ifs) ),
                      (std::istreambuf_iterator<char>()    ) );


    std::istrstream input_stream(data.c_str());

    std::multimap<std::string, std::string> record;

    //based on RFC5321

    static const std::set<std::string> target_file_type = {
            "application/octet-stream",
            "application/zip",
            "application/x-msdownload",
            "text/x-script.phyton"
    };

    static const std::regex         hello_command("(?:EHLO|HELO) (\\S+)");
    static const std::regex          auth_command("AUTH PLAIN (\\S+)");
    static const std::regex          mail_command("(?:MAIL|mail) FROM\\:\\s*(\\S+)");
    static const std::regex          rcpt_command("(?:RCPT|rcpt) TO\\:\\s*(\\S+)");
    static const std::regex          date_command("Date: (.+)");
    static const std::regex            ua_command("User-Agent: (.+)");
    static const std::regex      boundary_command("boundary=\"([\\w_=-]+)\"");
    static const std::regex  content_type_command("Content-[Tt]ype:\\s*([-/\\w]+)");
    static const std::regex          name_command("name=\"([-\\=\\+\?\\w\\.]+)\"");
    static const std::regex      encoding_command("Content-[Tt]ransfer-[Ee]ncoding: (\\w+)");
    static const std::regex          quit_command("(QUIT)");

    bool have_boundary = false;
    bool file_section = false;
    std::string line, s, boundary;
    std::string file_mime_type, file_name, file_encoding;

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
                std::cout << "get boundary  " << s << std::endl;
                break;
            }
        }
        for(const auto& iter: record) {
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
                    file_mime_type  = match(line, content_type_command);
                if (file_name == "")
                    file_name       = match(line, name_command);
                if (file_encoding == "")
                    file_encoding   = match(line, encoding_command);
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

            File* f = new File(
                    file_name,
                    file_mime_type,
                    file_encoding
            );


            if (file_encoding == "base64") {
                while (true) {
//                            std::getline(input_stream, line);
                    input_stream >> line;
                    if (is_boundary(line, boundary)) {
                        char ch;
                        do {
                            input_stream.get(ch);
                        } while (ch == '\n' || ch == '\r');
                        input_stream.putback(ch);
                        break;
                    }
                    if (line == "" || line == "\r") {
                        continue;
                    }
                    else {
                        f -> add_content(line);
                    }
                }
                std::cout << f ->get_name() << std::endl;
                std::cout << f ->get_mime_type() << std::endl;
                std::cout << f -> get_encoding() << std::endl;
                std::cout << f -> get_content() << std::endl << std::endl;
            }
            else {

            }

        }
    }
    catch (std::exception e){

    }



}

void test2() {

    std::string subject1 = "2333 UID fetch 1319733343,1319733344:1319733346,13242412 (UID RFC822.SIZE BODY.PEEK[])";
    std::string subject2 = "2333 UID fetch 13242412 (UID RFC822.SIZE BODY.PEEK[]<123,423>)";
    static const std::regex  multi_email("\\d* UID fetch ([\\d,:]*) \\(UID RFC822.SIZE BODY.PEEK\\[\\]\\)");
//    static const std::regex  part_email("\\d* UID fetch [\\d,:]* \\(UID RFC822.SIZE BODY.PEEK\\[\\]");
    static const std::regex    part_email("\\d* UID fetch [\\d,:]* \\(UID RFC822.SIZE BODY.PEEK\\[\\]\\<([\\d,]*)\\>\\)");
//    static const std::regex  determin_command("\\d* UID fetch ([\\d,:]*) \\(UID RFC822.SIZE BODY.PEEK\\[\\]\\)");
    std::string result;
    std::smatch match;

    std::string subject = subject2;
    std::string target_str;
    if (std::regex_search(subject, match, multi_email) && match.size() > 1) {
        std::cout << match.str(1) << std::endl;
        target_str = match.str(1);
        for (const auto& iter: split_str(target_str, ",")) {
            if (iter.find(":") != std::string::npos) {
                const auto& vec = split_str(iter, ":");
                std::cout << vec[0] << "   " << vec[1] << std::endl;
            }
            std::cout << iter << std::endl;
        }
    }
    else if (std::regex_search(subject, match, part_email) && match.size() > 1 ) {
        std::cout << match.size() << std::endl;
        std::cout << match.str(1) << std::endl;
        target_str = match.str(1);
        for (const auto& iter: split_str(target_str, ",")) {
            std::cout << iter << std::endl;
        }
    }
}

void test3() {

    std::ifstream ifs("imap_multi.log");
    std::string subject( (std::istreambuf_iterator<char>(ifs) ),
                      (std::istreambuf_iterator<char>()    ) );
    // std::string subject = "\r\n)\r\n* 2 FETCH (UID 1319733344 RFC822.SIZE 25447 BODY[] {25447}\r\n\r\ndawdawdawda\r\n)\r\n";
//    std::string subject = "abbbbbbba\r\n)\r\ndwadwadwad)\r\n";
    std::smatch match;

//    static const std::regex departer("a([\\S^\\s]*)");
    static const std::regex departer("\\* \\d* FETCH \\(UID \\d* RFC822.SIZE \\d* BODY\\[\\] \\{\\d*\\}([\\s^\\S]*?)\n\\)\r\n)");

    if (std::regex_search(subject, match, departer)) {
        std::cout << match.size() << std::endl;;
        std::cout << "get " << match.str(1) << std::endl;
    }
}

int main()
{

//    test();
//    test2();

    test3();


    return 0;

}
