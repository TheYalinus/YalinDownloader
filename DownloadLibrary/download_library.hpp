#ifndef DOWNLOAD_LIBRARY_HPP
#define DOWNLOAD_LIBRARY_HPP
#include <atomic>
#include <curl/curl.h>
#include <map>
#include <string>
#include <utility>
#include <cstdio>
#include <map>
#include <memory>
#include <string>
#include <iostream>
#include <curl/curl.h>
namespace DownloadLibrary {
    struct progress_data{
        std::atomic<int> total_download;
        std::atomic<int> stop_switch;
    };
    using RangeType = std::pair<std::string, std::string> ;
    class CurlRequest: std::enable_shared_from_this<CurlRequest>{
        private:
            std::string url;
            std::string range;
            std::string save_loc;
            bool header_only;
            FILE* file;
            CURL* curl;
            std::string user_agent;
            struct progress_data progress_data;


        public:
            CurlRequest(std::string url,RangeType range,std::string save_loc,bool header_only, std::string user_agent =" ");
            CURLcode curlPerform();
            void setUserAgent(std::string agent);
            static int progress_callback(void *clientp,
                                  curl_off_t  dltotal,
                                  curl_off_t  dlnow,
                                  curl_off_t  ultotal,
                                  curl_off_t  ulnow);
            int downloaded();
            void stop();
            ~CurlRequest();
    };
}


#endif
