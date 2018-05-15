#include "function.hpp"
#include "cuckoo_sniffer.hpp"

#include <sstream>
#include <iomanip>
#include <set>

#include <openssl/md5.h>
#include <curl/curl.h>

#include "tins/tcp_ip/stream_follower.h"
#include "tins/ip_address.h"
#include "tins/ipv6_address.h"

#include "util/file.hpp"
#include "util/buffer.hpp"

namespace cs::util {

    using namespace std;

    vector<string> split_str(const string str, const string split) {
        string::size_type last_pos = 0, new_pos;
        string::size_type spliter_len = split.size();
        vector<string> vec;
        while (1) {
            new_pos = str.find(split, last_pos);
            vec.push_back(str.substr(last_pos, new_pos - last_pos));
            if (new_pos == string::npos) {
                return vec;
            }
            else {
                last_pos = new_pos + spliter_len;
            }
        }
    }


    string stream_identifier(const Tins::TCPIP::Stream& stream) {
        ostringstream output;
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

    bool is_ignore_stream(const Tins::TCPIP::Stream& stream, const std::set<Tins::IPv4Address>& ignore_ipv4_address, const std::set<Tins::IPv6Address>& ignore_ipv6_address) {
        if (stream.is_v6()) {
            auto addr = stream.client_addr_v6();
            auto iter = ignore_ipv6_address.find(addr);
            return iter != ignore_ipv6_address.end();
        }
        else {
            auto addr = stream.client_addr_v4();
            auto iter = ignore_ipv4_address.find(addr);
            return iter != ignore_ipv4_address.end();
        }
    }

    string md5(const char* data, size_t size) {
        unsigned char md[16];
        ostringstream os;
        MD5(reinterpret_cast<const unsigned char*>(data), size, md);
        os << setw(2) << setfill('0') << hex << uppercase;
        for (int i = 0; i < 16; ++i) {
            os << setw(2) << static_cast<int>(md[i]);
        }
        return os.str();
    }

    int submit_files(const vector<File*>& file_vec) {
        curl_global_init(CURL_GLOBAL_ALL);

        for (auto f: file_vec) {
            submit_files(*f, cs::CONFIG["submit_url"]);
        }
    }


    int submit_files(const File& file, const string& url)
    {

        CURL *curl;
        CURLcode res;
        struct curl_httppost *formpost=NULL;
        struct curl_httppost *lastptr=NULL;

        /* In windows, this will init the winsock stuff */

        /* get a curl handle */
        curl = curl_easy_init();
        if(curl) {
            /* First set the URL that is about to receive our POST. This URL can
            just as well be a https:// URL if that is what should receive the
            data. */
            curl_easy_setopt(curl, CURLOPT_URL, url.data());
            /* Now specify the POST data */
//        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "file=daniel&project=curl");

            const string& name = file.get_name();
            const string& mime_type = file.get_mime_type();


            curl_formadd(
                    &formpost,
                    &lastptr,
                    CURLFORM_COPYNAME, "file",
                    CURLFORM_BUFFER, name.c_str(),
                    CURLFORM_BUFFERPTR, file.get_buffer()->data_to_read(),
                    CURLFORM_BUFFERLENGTH, file.get_size(),
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
        return 0;

    }

}
