#include "download_library.hpp"
#include <cmath>
#include <cstdlib>
#include <curl/curl.h>
#include <curl/easy.h>
#include <exception>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <nlohmann/json.hpp>
#include <ostream>
#include <stdexcept>
#include <string>
std::string DownloadLibrary::DownloadTask::gen_part_name(int i){

    return this->final_props.file_name+ "_" + "[" + std::to_string(i) + "]";
}
void DownloadLibrary::DownloadTask::create_json_config(json &n){
    n["version"]=1.0;
    n["parts"]=json::object();
    n["url"]=this->url;
    n["total_size"]=this->total_bytes;
    std::cout<<n<<std::endl;
}
void DownloadLibrary::DownloadTask::create_json_config(){
    this->cfg_data["version"]=1.0;
    this->cfg_data["parts"]=json::object();
    this->cfg_data["url"]=this->url;
    this->cfg_data["total_size"]=this->total_bytes;
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
DownloadLibrary::DownloadTask::DownloadTask(std::string url,std::string task_location, int part_count, std::string user_agent, bool follow_redirects ,struct file_properties final_props):
url(url),part_count(part_count),final_props(final_props), user_agent(user_agent),parts(part_count),follow_redirects(follow_redirects){
    if(!std::filesystem::exists(task_location))
        throw std::runtime_error("Task directory does not exists");
    else if (!std::filesystem::is_directory(task_location))
        throw std::runtime_error("Specified path is not directory.");
    else if(!std::filesystem::is_empty(task_location))
        throw std::runtime_error("Task directory should be empty.");

    if(final_props.file_name.empty())
    {
        std::cout<<"Neler oluyor bak hayatta"<<std::endl;
        this->final_props= get_inf();
    }
    else {
        get_inf();
    }
    this->cfg_data["cnt-type"]= this ->cnttype;
    this->cfg_data["file-props"]["filename"]= this->final_props.file_name;
    this->cfg_data["file-props"]["fileextension"]= this->final_props.file_extension;
    if(this->total_bytes == -1)
        throw std::runtime_error("Server doesnt gives the header info or link is broken");
    this->task_dir = std::filesystem::path(task_location);
    this->cfg_path= task_dir / "download.json";
    create_json_config();
    long def_part_size= total_bytes / part_count;
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
            std::make_shared<DownloadLibrary::CurlRequest>(this->url,task_dir / gen_part_name(i),false,buff,this->user_agent),
            0
        };
        this->cfg_data["parts"][std::to_string(i)]["range"]=buff.get_range();
    }
    std::ofstream cfg(this->cfg_path);
    cfg<<this->cfg_data;
}
struct DownloadLibrary::file_properties DownloadLibrary::DownloadTask::get_inf(){
    curl_off_t total_size_buffer=0;
    std::string content_type_buff;
    char * effective_url_buff_c;

    struct curl_header *type;

    std::string full_file_name="random";
    CURL * curl= curl_easy_init();
    std::string name_bff;
    curl_easy_setopt(curl, CURLOPT_USERAGENT, this->user_agent.c_str());
    curl_easy_setopt(curl, CURLOPT_URL, this->url.c_str());
    curl_easy_setopt(curl, CURLOPT_NOBODY, 1L);
    if(this->follow_redirects)
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    if (curl_easy_perform(curl)!=CURLE_OK)
        throw std::runtime_error("Cant connect to the server at "+this->url);
    curl_easy_getinfo(curl, CURLINFO_CONTENT_LENGTH_DOWNLOAD_T, &total_size_buffer);
    curl_easy_header(curl, "Content-Type", 0, CURLH_HEADER, -1, &type);
    curl_easy_getinfo(curl, CURLINFO_EFFECTIVE_URL, &effective_url_buff_c);

    if(effective_url_buff_c == NULL){
        std::cerr<<"Libcurl has not given the effective url, so name will be random";
    }
    else{
        std::string effective_url_buff(effective_url_buff_c);
        full_file_name = effective_url_buff.substr(effective_url_buff.find_last_of("/")+1,(effective_url_buff.find('?')-(effective_url_buff.find_last_of("/")))-1);
    }
    this->cnttype = std::to_string(*type->value);
    this->total_bytes = total_size_buffer;
    curl_easy_cleanup(curl);
    return {
        full_file_name.substr(full_file_name.find_last_of(".")+1),
        full_file_name
    };

}
