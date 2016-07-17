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
#include "util/mail_process.hpp"

void test() {
    std::ifstream ifs("tb_smtp.log");
    std::string data( (std::istreambuf_iterator<char>(ifs) ),
                      (std::istreambuf_iterator<char>()    ) );
    mail_process(data);
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

//    std::ifstream ifs("imap_multi.log");
//    std::string subject( (std::istreambuf_iterator<char>(ifs) ),
//                      (std::istreambuf_iterator<char>()    ) );
    // std::string subject = "\r\n)\r\n* 2 FETCH (UID 1319733344 RFC822.SIZE 25447 BODY[] {25447}\r\n\r\ndawdawdawda\r\n)\r\n";
    std::string subject = "\r\nC9 UID FETCH 131923 (UID BODY.PEEK[])\r\n";
    std::smatch match;

//    static const std::regex departer("a([\\S^\\s]*)");
    static const std::regex departer("\\* \\d* FETCH \\(UID \\d* RFC822.SIZE \\d* BODY\\[\\] \\{\\d*\\}([\\s^\\S]*?)\n\\)\r\n)");
    static const std::regex  multi_email("[\\w]* UID (?:fetch)|(?:FETCH) ([\\d,:]*) \\(UID (?:RFC822.SIZE )?BODY.PEEK\\[\\]\\)");

    if(regex_search(subject, match, multi_email)) {
        std::cout << "get \n\n" << match.str(1) << std::endl;
    }
}

int main()
{

//    test();
//    test2();

    test3();


    return 0;

}
