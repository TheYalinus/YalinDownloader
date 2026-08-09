#ifndef DOWNLOAD_LIBRARY_HPP
#define DOWNLOAD_LIBRARY_HPP
#include <atomic>
#include <curl/curl.h>
#include <filesystem>
#include <map>
#include <nlohmann/json.hpp>
#include <string>
#include <utility>
#include <cstdio>
#include <map>
#include <memory>
#include <string>
#include <iostream>
#include <curl/curl.h>
#include <vector>
namespace DownloadLibrary {
    struct progress_data{
        std::atomic<long> total_download;
        std::atomic<int> stop_switch;
    };

    using RangeType = std::pair<std::string, std::string> ;
    using json =nlohmann::json;
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
            CurlRequest(std::string url,RangeType range,std::string save_loc,bool header_only, std::string user_agent =" ", bool follow_redirects=false);
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
    struct part_data{
        std::shared_ptr<CurlRequest> req;
        long downloaded_bytes;
        std::string range;
    };
    struct file_properties{
        std::string file_extension;
        std::string file_name;
    };
    class DownloadTask{
        private:
            int part_count;
            long total_bytes;
            long total_downloaded_bytes;
            std::filesystem::path task_dir;
            std::filesystem::path cfg_path;
            std::vector<part_data> parts;
            std::string url;
            std::string user_agent;
            json cfg_data;
        public:
            DownloadTask(std::string task_location,std::string user_agent="");
            DownloadTask(std::string url,std::string task_location, int part_count, struct file_properties={}, std::string user_agent="");
            void get_inf(std::string url);
    };

}


#endif
