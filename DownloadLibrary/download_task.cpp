#include "download_library.hpp"
#include <chrono>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <ctime>
#include <curl/curl.h>
#include <curl/easy.h>
#include <exception>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
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

}
void DownloadLibrary::DownloadTask::create_json_config(){
    this->cfg_data["version"]=1.0;
    this->cfg_data["parts"]=json::object();
    this->cfg_data["url"]=this->url;
    this->cfg_data["total_size"]=this->total_bytes;
}
DownloadLibrary::DownloadTask::DownloadTask(std::string task_location,std::string user_agent, std::string dns):user_agent(user_agent){
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
    this->cfg_data=  json::parse(cfgstream);//json::parse_error should be catched by class creator
    this->url = cfg_data["url"];
    inf_data id = get_inf();
    if(cfg_data["cnt-type"] != id.cnttype || cfg_data["total_size"] != id.totalbytes)
        throw std::runtime_error("Broken link");
    this->cnttype = cfg_data["cnt-type"];
    this->final_props.file_name = cfg_data["file-props"]["filename"];
    this->final_props.file_extension = cfg_data["file-props"]["filename"];
    this->total_bytes= cfg_data["total_size"];
    for( auto n : cfg_data["parts"]){
        std::uintmax_t filesize=std::filesystem::file_size(this->task_dir / n["name"]);
        int id = n["id"];
        if(n["size"]==filesize)
            continue;
        RangeType buff (n["range"]);
        std::cout<<buff.first<<buff.second <<std::endl;
        /*buff.first= std::to_string(stol(buff.first) + filesize);*/
        this->parts.push_back({
            std::make_shared<DownloadLibrary::CurlRequest>(this->url, n["name"],false,buff,this->user_agent,dns),
            id,
            static_cast<long>(filesize)
        });
    }


}
DownloadLibrary::DownloadTask::DownloadTask(std::string url,std::string task_location, int part_count, std::string user_agent, std::string dns,  bool follow_redirects ,struct file_properties final_props):
url(url),part_count(part_count),final_props(final_props), user_agent(user_agent),parts(part_count),follow_redirects(follow_redirects){
    if(!std::filesystem::exists(task_location))
        throw std::runtime_error("Task directory does not exists");
    else if (!std::filesystem::is_directory(task_location))
        throw std::runtime_error("Specified path is not directory.");
    else if(!std::filesystem::is_empty(task_location))
        throw std::runtime_error("Task directory should be empty.");

    struct inf_data id= get_inf();
    if(final_props.file_name.empty())
    {
        this->final_props= id.props;
    }
    this->total_bytes = id.totalbytes;
    this->cnttype = id.cnttype;
    this->cfg_data["cnt-type"]= this ->cnttype;
    this->cfg_data["file-props"]["filename"]= this->final_props.file_name;
    this->cfg_data["file-props"]["fileextension"]= this->final_props.file_extension;
    if(this->total_bytes == -1)
        throw std::runtime_error("Server doesnt gives the header info or link is broken");
    this->task_dir = std::filesystem::path(task_location);
    this->cfg_path= task_dir / "download.json";
    create_json_config();
    long def_part_size= total_bytes / part_count;
    long remainder = total_bytes % part_count;
    std::cout<<"remains "<<remainder<<std::endl;
    for(int i=0;i<part_count;i++)
    {
        RangeType buff("","");
        if(i<part_count-1)
        {
            buff.first=  std::to_string(i*def_part_size);
            buff.second= std::to_string(((i+1)*def_part_size)-1);
            this->cfg_data["parts"][std::to_string(i)]["size"]=def_part_size;
        }
        else{
            buff.first=  std::to_string(i*def_part_size);
            this->cfg_data["parts"][std::to_string(i)]["size"]=def_part_size+remainder;
        }
        parts[i]={
            std::make_shared<DownloadLibrary::CurlRequest>(this->url,task_dir / gen_part_name(i),false,buff,this->user_agent,dns),
            i,
            0
        };
        this->cfg_data["parts"][std::to_string(i)]["range"]=buff.get_range();
        this->cfg_data["parts"][std::to_string(i)]["name"]= gen_part_name(i);
        this->cfg_data["parts"][std::to_string(i)]["id"] = i;    }
    std::ofstream cfg(this->cfg_path);
    cfg<<this->cfg_data;
}
struct DownloadLibrary::inf_data DownloadLibrary::DownloadTask::get_inf(){
    curl_off_t total_size_buffer=0;
    std::string extension;
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
    content_type_buff = type->value;

    if(effective_url_buff_c == NULL){
        std::cerr<<"Libcurl has not given the effective url, so name will be random";
        extension="";
        //random name gen
        full_file_name  = gen_random_file_name();
    }
    else{
        std::string effective_url_buff(effective_url_buff_c);
        full_file_name = effective_url_buff.substr(effective_url_buff.find_last_of("/")+1,(effective_url_buff.find('?')-(effective_url_buff.find_last_of("/")))-1);
        extension = full_file_name.substr(full_file_name.find_last_of(".")+1);
    }
    curl_easy_cleanup(curl);
    return {
        {extension,
        full_file_name},
        content_type_buff,
        total_size_buffer

        };
}
std::vector<DownloadLibrary::part_data> DownloadLibrary::DownloadTask::get_parts(){
    return this->parts;
}
void DownloadLibrary::DownloadTask::assemble(){

    std::ofstream final_file (this->task_dir / this->final_props.file_name);
    for (auto n : this->cfg_data["parts"])
    {
            std::string name =n["name"];
            std::filesystem::path a (this->task_dir / name);
            std::cout<<std::filesystem::file_size(a)<< std::endl;
            std::ifstream strm(a);
            final_file<<strm.rdbuf();

    }


}

std::string DownloadLibrary::DownloadTask::gen_random_file_name(){
    //some random name generator will be implemented
    return "Downloaded_file";
}
