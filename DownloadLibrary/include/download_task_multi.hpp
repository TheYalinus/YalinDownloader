#ifndef DOWNLOAD_TASK_MULTI_HPP
#define DOWNLOAD_TASK_MULTI_HPP
#include "download_task.hpp"
#include "connection_pool.hpp"
#include <memory>
namespace DownloadLibrary {
    class DownloadTaskMultiple: public DownloadLibrary::DownloadTask{
        private:
            sharedPoolType connectionPool;
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
            DownloadTaskMultiple(std::string task_location,sharedPoolType connectionPool,std::string user_agent="", std::string dns="", bool factory_flag = false);
            DownloadTaskMultiple(HEADER_FLAG header ,sharedPoolType connectionPool, factory_data fdata ,std::string url,std::string task_location, int part_count, std::string user_agent="", std::string dns="" ,bool follow_redirects = true, struct file_properties={"",""});
            ~DownloadTaskMultiple();
            void create_json_config(json & n);
            void create_json_config();
            std::vector<part_data> get_parts();
            void assemble() override;
            void clean() override;
            ReqsType get_requests() override;
    };
}
#endif
