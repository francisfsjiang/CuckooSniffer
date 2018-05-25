#include "function.hpp"
#include "cuckoo_sniffer.hpp"

#include <sstream>
#include <iomanip>
#include <set>

#include <openssl/md5.h>
#include <curl/curl.h>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "util/file.hpp"
#include "util/buffer.hpp"
#include "base/sniffer.hpp"

namespace cs::util {

    using namespace std;
    using namespace cs::base;

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


    bool is_ignored_stream(const StreamIdentifier& stream_id, const std::set<std::string>& ignore_addr_set) {

        auto iter = ignore_addr_set.find(stream_id.src_addr);
        return iter != ignore_addr_set.end();
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

    int submit_files_and_delete(vector<File *>& file_vec) {
        curl_global_init(CURL_GLOBAL_ALL);

        for (int i = 0; i < file_vec.size(); ++i) {

            submit_file_and_delete(file_vec[i], cs::CONFIG["submit_url"]);
        }
    }


    int submit_file_and_delete(File* &file) {
        submit_file_and_delete(file, cs::CONFIG["submit_url"]);
    }

    size_t noop_cb(void *ptr, size_t size, size_t nmemb, void *data) {
        return size * nmemb;
    }

    string get_custom_data(File* file) {
        using namespace boost::property_tree;
        ptree root;
        root.put("src_ip", file->ip_.src_addr);
        root.put("src_port", file->ip_.src_port);
        root.put("dst_ip", file->ip_.dst_addr);
        root.put("dst_port", file->ip_.dst_port);

        stringstream is;
        write_json(is ,root);
        return is.str();
    }

    int submit_file_and_delete(File* &file, const string &url)
    {
        LOG_DEBUG << "Sending file, " << file->get_name() << " , " << file->get_size() << " , " << file->get_md5();
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
            curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L);
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, noop_cb);
            /* Now specify the POST data */
//        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "file=daniel&project=curl");

            if(file->name_.empty()) {
                file->name_ = file->get_md5();
            }

            string custom_data = get_custom_data(file);
            curl_formadd(&formpost,
                         &lastptr,
                         CURLFORM_COPYNAME, "custom",
                         CURLFORM_COPYCONTENTS, custom_data.c_str(),
                         CURLFORM_END);

            curl_formadd(
                    &formpost,
                    &lastptr,
                    CURLFORM_COPYNAME, file->name_.data(),
                    CURLFORM_BUFFER, file->name_.c_str(),
                    CURLFORM_BUFFERPTR, file->get_buffer()->data_to_read(),
                    CURLFORM_BUFFERLENGTH, file->get_size(),
                    CURLFORM_CONTENTTYPE, file->mime_type_.c_str(),
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
        delete file;
        return 0;

    }

}
