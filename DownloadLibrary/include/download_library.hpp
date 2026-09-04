#ifndef DOWNLOAD_LIBRARY_HPP
#define DOWNLOAD_LIBRARY_HPP
#include <atomic>
#include <cstddef>
#include <curl/curl.h>
#include <curl/easy.h>
#include <curl/header.h>
#include <curl/system.h>
#include <exception>
#include <filesystem>
#include <fstream>
#include <future>
#include <map>
#include <nlohmann/json.hpp>
#include <string>
#include <thread>
#include <utility>
#include <cstdio>
#include <map>
#include <memory>
#include <string>
#include <iostream>
#include <curl/curl.h>
#include <vector>
namespace DownloadLibrary {

    enum HEADER_FLAG{
        HEADER_ACCEPT,
        HEADER_REJECT
    };
    class TaskException : public std::exception{
        private:
            std::string message;
            CURLcode ccode;
        public:
            TaskException(CURLcode code, std::string message="");



            const char* what() noexcept{
                return message.c_str();
            }
            CURLcode code() noexcept{
                return ccode;
            }


    };
    struct progress_data{
        std::atomic<long> total_download;
        std::atomic<int> stop_switch=0;
    };


    class RangeType: public std::pair<std::string, std::string>{
        public:
            RangeType(std::string a, std::string b);
            RangeType(std::string a);
            std::string get_range();
    };
    using json =nlohmann::json;
    class CurlRequest;

    struct part_data{
        std::shared_ptr<CurlRequest> req;
        int part_id ;
        long downloaded_bytes;
    };
    struct file_properties{ //For final output file
        std::string file_extension;
        std::string file_name;
    };
    struct inf_data{
        file_properties props;
        std::string cnttype;
        long totalbytes;
    };
    struct factory_data{
        curl_off_t total_size;
        std::string cnt_type;
        std::string efct_url;
        file_properties props;
    };
    class DownloadTask;
    class DownloadFactory;
    using ReqsType = std::vector<std::shared_ptr<DownloadLibrary::CurlRequest>>;
    using ThreadsType = std::vector<std::shared_ptr<std::thread>>;
    class ParallelDownloader;



}


#endif
