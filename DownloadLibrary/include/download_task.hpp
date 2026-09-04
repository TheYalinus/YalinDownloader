#ifndef DOWNLOAD_TASK_HPP
#define DOWNLOAD_TASK_HPP
#include "download_library.hpp"
#include "curl_request.hpp"
namespace DownloadLibrary {
    class DownloadTask{
        protected:
            std::string gen_part_name(int i);
            std::string dns;
            struct inf_data get_inf();
            static std::string gen_random_file_name();
            long total_bytes;
            long total_downloaded_bytes;
            std::filesystem::path task_dir;
            std::string url;
            std::string user_agent;
        public:

            struct file_properties final_props;
            bool follow_redirects;
            DownloadTask(std::string task_location, std::string user_agent="", std::string dns="", file_properties final_props = {"",""});
            virtual std::vector<std::shared_ptr<DownloadLibrary::CurlRequest>> get_requests()=0;
            virtual void assemble()=0;
            virtual void clean()=0;
            virtual ~DownloadTask();
    };
    class DownloadTaskMultiple;

}
#endif
