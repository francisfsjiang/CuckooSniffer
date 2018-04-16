#include "function.hpp"

#include <sstream>
#include <iomanip>

#include <openssl/md5.h>
#include <curl/curl.h>

#include "tins/tcp_ip/stream_follower.h"
#include "tins/ip_address.h"
#include "tins/ipv6_address.h"

#include "util/file.hpp"

namespace cs::util {

std::vector<std::string> split_str(const std::string str, const std::string split) {
    std::string::size_type last_pos = 0, new_pos;
    std::string::size_type spliter_len = split.size();
    std::vector<std::string> vec;
    while (1) {
        new_pos = str.find(split, last_pos);
        vec.push_back(str.substr(last_pos, new_pos - last_pos));
        if (new_pos == std::string::npos) {
            return vec;
        }
        else {
            last_pos = new_pos + spliter_len;
        }
    }
}


std::string stream_identifier(const Tins::TCPIP::Stream &stream) {
    std::ostringstream output;
    if (stream.is_v6()) {
        output << stream.client_addr_v6()
               << ":"
               << stream.client_port()
               << "->"
               << stream.server_addr_v6()
               << ":"
               << stream.server_port();
    }
    else {
        output << stream.client_addr_v4()
               << ":"
               << stream.client_port()
               << "->"
               << stream.server_addr_v4()
               << ":"
               << stream.server_port();
    }
    return output.str();
}

std::string md5(const char* data, size_t size) {
    unsigned char md[16];
    std::ostringstream os;
    MD5(reinterpret_cast<const unsigned char*>(data), size, md);
    os << std::setw(2) << std::setfill('0') << std::hex << std::uppercase;
    for (int i = 0; i < 16; ++i) {
        os << std::setw(2) << static_cast<int>(md[i]);
    }
    return os.str();
}



int submit_file(const File& f, const char* url)
{

    CURL *curl;
    CURLcode res;
    struct curl_httppost *formpost=NULL;
    struct curl_httppost *lastptr=NULL;

    /* In windows, this will init the winsock stuff */
    curl_global_init(CURL_GLOBAL_ALL);

    /* get a curl handle */
    curl = curl_easy_init();
    if(curl) {
        /* First set the URL that is about to receive our POST. This URL can
        just as well be a https:// URL if that is what should receive the
        data. */
        curl_easy_setopt(curl, CURLOPT_URL, url);
        /* Now specify the POST data */
//        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "file=daniel&project=curl");

        const std::string& name = f.get_name();
        const std::string& mime_type = f.get_mime_type();


        curl_formadd(
                &formpost,
                &lastptr,
                CURLFORM_COPYNAME, "file",
                CURLFORM_BUFFER, name.c_str(),
                CURLFORM_BUFFERPTR, f.get_buffer(),
                CURLFORM_BUFFERLENGTH, f.get_size(),
                CURLFORM_CONTENTTYPE, mime_type.c_str(),
                CURLFORM_END
        );

        curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);


        /* Perform the request, res will get the return code */
        res = curl_easy_perform(curl);
        /* Check for errors */
        if(res != CURLE_OK)
        fprintf(stderr, "curl_easy_perform() failed: %s\n",
        curl_easy_strerror(res));

        /* always cleanup */
        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();
    return 0;

}

}
