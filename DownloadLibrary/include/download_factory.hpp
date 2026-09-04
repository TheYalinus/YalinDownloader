#ifndef DOWNLOAD_FACTORY_HPP
#define DOWNLOAD_FACTORY_HPP
#include "download_library.hpp"
namespace DownloadLibrary {
    class DownloadFactory{
        private:
            static factory_data gather_head(CURL * curl);
            static factory_data gather_get(CURL * curl);
        public:
            static size_t dumm_write_callback(char *ptr, size_t size, size_t nmemb, void *userdata);
            static DownloadTask* createTask(std::string url, std::string task_location , int part_count , std::string user_agent = "",  bool follow_redirects = true , file_properties props={"",""} , std::string dns ="");
            static DownloadTask * createTask(std::string task_location, std::string user_agent="" , std::string dns="", bool follow_redirects=true);
            static int dumm_progress_callback(void *clientp,
                                  curl_off_t  dltotal,
                                  curl_off_t  dlnow,
                                  curl_off_t  ultotal,
                                  curl_off_t  ulnow);

    };

}
#endif
