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
            std::fstream stream;


        public:
            CurlRequest(std::string url,std::string save_loc,bool header_only,RangeType range = {"",""},  std::string user_agent =" ", std::string dns ="", bool follow_redirects=true);
            CURLcode curlPerform();
            void setUserAgent(std::string agent);
            static int progress_callback(void *clientp,
                                  curl_off_t  dltotal,
                                  curl_off_t  dlnow,
                                  curl_off_t  ultotal,
                                  curl_off_t  ulnow);
            static size_t write_function(char * data , size_t size , size_t nmemb, void * clientp);
            int downloaded();
            void stop();
            ~CurlRequest();
    };
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
    class DownloadTask{
        public:
            long total_bytes;
            long total_downloaded_bytes;
            std::filesystem::path task_dir;
            std::string url;
            std::string user_agent;
            struct file_properties final_props;
            bool follow_redirects;
            DownloadTask(std::string task_location, std::string user_agent="", std::string dns="", file_properties final_props = {"",""});
            struct inf_data get_inf();
            static std::string gen_random_file_name();
            std::string gen_part_name(int i);
            std::string dns;

    };
    class DownloadFactory{
        private:
            static factory_data gather_head(CURL * curl);
            static factory_data gather_get(CURL * curl);
        public:
             static size_t dumm_write_callback(char *ptr, size_t size, size_t nmemb, void *userdata);
            static DownloadTask* createTask(std::string url, std::string task_location , int part_count ,  bool follow_redirects = true , file_properties props={"",""} ,std::string user_agent = "", std::string dns ="");
            static int dumm_progress_callback(void *clientp,
                                  curl_off_t  dltotal,
                                  curl_off_t  dlnow,
                                  curl_off_t  ultotal,
                                  curl_off_t  ulnow);

    };
    class DownloadTaskMultiple: public DownloadLibrary::DownloadTask{
        private:
            HEADER_FLAG header;
            int part_count;
            long total_bytes;
            long total_downloaded_bytes;
            std::filesystem::path task_dir;
            std::filesystem::path cfg_path;
            std::vector<part_data> parts;
            std::string url;
            std::string user_agent;
            json cfg_data;
            bool follow_redirects;
            std::string cnttype;
        public:
            DownloadTaskMultiple(std::string task_location,std::string user_agent="", std::string dns="");
            DownloadTaskMultiple(HEADER_FLAG header ,factory_data fdata ,std::string url,std::string task_location, int part_count, std::string user_agent="", std::string dns="" ,bool follow_redirects = true, struct file_properties={"",""});
            void create_json_config(json & n);
            void create_json_config();
            std::vector<part_data> get_parts();
            void assemble();

    };

}


#endif
