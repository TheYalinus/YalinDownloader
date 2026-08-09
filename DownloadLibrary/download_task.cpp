#include "download_library.hpp"
#include <curl/curl.h>
#include <curl/easy.h>
#include <filesystem>
#include <fstream>
#include <memory>
#include <nlohmann/json.hpp>
#include <stdexcept>
#include <string>

void DownloadLibrary::DownloadTask::create_json_config(json &n){
    n["version"]=1.0;
    n["parts"]=json::object();
    n["url"]=this->url;
    n["total_size"]=this->total_bytes;
    std::cout<<n<<std::endl;
}
DownloadLibrary::DownloadTask::DownloadTask(std::string task_location,std::string user_agent):user_agent(user_agent){
    if(!std::filesystem::exists(task_location))
        throw std::runtime_error("Directory does not exists");
    else if (!std::filesystem::is_directory(task_location))
        throw std::runtime_error("Specified path is not directory.");
    else if(std::filesystem::is_empty(task_location))
        throw std::runtime_error("Directory is empty.");
    this->task_dir = std::filesystem::path( task_location );
    this->cfg_path= task_dir / "download.json";
    if(!std::filesystem::exists(cfg_path))
        throw std::runtime_error("There is no download.json file in specified directory");
    std::fstream cfgstream(cfg_path);
    this->cfg_data=  json::parse(cfgstream); //json::parse_error should be catched
}
DownloadLibrary::DownloadTask::DownloadTask(std::string url,std::string task_location, int part_count, std::string user_agent, struct file_properties final_props):
url(url),part_count(part_count),final_props(final_props), user_agent(user_agent),parts(part_count){
    if(!std::filesystem::exists(task_location))
        throw std::runtime_error("Task directory does not exists");
    else if (!std::filesystem::is_directory(task_location))
        throw std::runtime_error("Specified path is not directory.");
    else if(!std::filesystem::is_empty(task_location))
        throw std::runtime_error("Task directory should be empty.");

    if(final_props.file_name.empty())
    {
        this->final_props= get_inf();
    }
    else {
        get_inf();
    }
    if(this->total_bytes == -1)
        throw std::runtime_error("Server doesnt gives the header info or link is broken");
    this->task_dir = std::filesystem::path(task_location);
    this->cfg_path= task_dir / "download.json";
    json n= cfg_data;
    create_json_config(n);
    //write_parts_info

    long def_part_size= total_bytes / part_count;
    std::cout<<def_part_size<<std::endl;

    for(int i=0;i<part_count;i++)
    {
        RangeType buff("","");
        if(i<part_count-1)
        {
            buff.first=  std::to_string(i*def_part_size);
            buff.second= std::to_string((i+1)*def_part_size);
        }
        else{
            buff.first=  std::to_string(i*def_part_size);
        }

        parts[i]={
            std::make_shared<DownloadLibrary::CurlRequest>(this->url,this->final_props.file_name+this->final_props.file_extension,false,buff,this->user_agent),
            0
        };
        n["parts"][std::to_string(i)]["range"]=buff.get_range();
    }
    std::cout<<n<<std::endl;
}
DownloadLibrary::file_properties DownloadLibrary::DownloadTask::get_inf(){
    curl_off_t total_size_buffer=0;
    CURL * curl= curl_easy_init();
    std::string name_bff;
    curl_easy_setopt(curl, CURLOPT_USERAGENT, this->user_agent.c_str());
    curl_easy_setopt(curl, CURLOPT_URL, this->url.c_str());
    curl_easy_setopt(curl, CURLOPT_NOBODY, 1L);
    curl_easy_perform(curl);
    curl_easy_getinfo(curl, CURLINFO_CONTENT_LENGTH_DOWNLOAD_T, &total_size_buffer);
    curl_easy_cleanup(curl);
    this->total_bytes = total_size_buffer;
    return {
        "a",
        "a"
    };
}
