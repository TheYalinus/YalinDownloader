#include "download_library.hpp"
#include <curl/curl.h>
#include <curl/easy.h>
#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>
#include <stdexcept>
DownloadLibrary::DownloadTask::DownloadTask(std::string task_location,std::string user_agent):user_agent(user_agent){
    if(!std::filesystem::exists(task_location))
        throw std::runtime_error("Directory does not exists");
    else if (!std::filesystem::is_directory(task_location))
        throw std::runtime_error("Specified path is not directory.");
    else if(std::filesystem::is_empty(task_location))
        throw std::runtime_error("Directory is empty.");
    this->task_dir = std::filesystem::path( task_location );
    this->cfg_path= task_dir /= "download.json";
    if(!std::filesystem::exists(cfg_path))
        throw std::runtime_error("There is no download.json file in specified directory");
    std::fstream cfgstream(cfg_path);

    this->cfg_data=  json::parse(cfgstream); //json::parse_error should be catched


}
void DownloadLibrary::DownloadTask::get_inf(std::string url){
    curl_off_t total_size_buffer=0;
    this->url =url;
    CURL * curl= curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_USERAGENT, this->user_agent.c_str());
    curl_easy_setopt(curl, CURLOPT_URL, this->url.c_str());
    curl_easy_setopt(curl, CURLOPT_NOBODY, 1L);
    curl_easy_perform(curl);
    curl_easy_getinfo(curl, CURLINFO_CONTENT_LENGTH_DOWNLOAD_T, &total_size_buffer);
    curl_easy_cleanup(curl);
    std::cout<<total_size_buffer<<std::endl;
}
