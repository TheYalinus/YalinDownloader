#include "download_library.hpp"
#include "curl_request.hpp"
#include <chrono>
#include <curl/curl.h>
#include <curl/easy.h>
#include <curl/system.h>
#include <filesystem>
#include <fstream>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <thread>


DownloadLibrary::CurlRequest::CurlRequest(std::string url, std::string save_loc , bool header_only, RangeType range,  std::string user_agent, std::string dns , bool follow_redirects):
Url(url), saveLoc(save_loc), isHdrOnly(header_only),curl(curl_easy_init()),prgData{0,0},dataStream(std::fstream()){
        curl_easy_setopt(this->curl, CURLOPT_XFERINFODATA, reinterpret_cast<void *>(&this->prgData));
        curl_easy_setopt(this->curl, CURLOPT_NOPROGRESS, 0L);
        curl_easy_setopt(this->curl, CURLOPT_XFERINFOFUNCTION, progress_callback);
        curl_easy_setopt(this->curl, CURLOPT_WRITEFUNCTION, write_function);

        if(follow_redirects)

            curl_easy_setopt(this->curl, CURLOPT_FOLLOWLOCATION, 1L);
        if(user_agent != " "){
            this->usrAgent = user_agent;
            curl_easy_setopt(this->curl, CURLOPT_USERAGENT, user_agent.c_str());
        }
        if(!std::filesystem::exists(this->saveLoc))
        {std::ofstream a(this->saveLoc); a.close();}

        this->dataStream.open(this->saveLoc, std::ios::app);
        std::cout<<"Curl Request Debug 2: "<<url<< this->dataStream.is_open() <<std::endl;
        /*this->file = fopen(this->save_loc.c_str(), "a+");
        if(this->file == nullptr){
            std::cout<<"The file cannot be opened or does not exists"<<std::endl;
            std::ofstream x (this->save_loc);
            this->file = fopen(this->save_loc.c_str(), "w+");
            }*/
        curl_easy_setopt(this->curl, CURLOPT_FILE, &this->dataStream);

        if(range.first != "ignore"){

            curl_easy_setopt(this->curl,  CURLOPT_RANGE, range.get_range().c_str());
        }
        if (!dns.empty())
            curl_easy_setopt(this->curl, CURLOPT_DNS_SERVERS, dns.c_str());

        curl_easy_setopt(this->curl, CURLOPT_URL, this->Url.c_str());

}
CURLcode DownloadLibrary::CurlRequest::curlPerform(){
    std::cout<<"curlPerform"<<std::endl;
    auto cc =curl_easy_perform(this->curl);
    long http_code=0;
    curl_easy_getinfo(this->curl, CURLINFO_RESPONSE_CODE, &http_code);
    std::cout<<cc<<"-"<<http_code<<std::endl;
    while(http_code == 429){
        std::cout<<"429"<<std::endl;
        struct curl_header *retry_after;
        std::filesystem::remove(std::filesystem::path(this->saveLoc));
        this->dataStream.close();
        this->dataStream.clear();
        std::ofstream a(this->saveLoc);
        a.close();
        curl_easy_header(curl, "Retry-After", 0, CURLH_HEADER, -1, &retry_after);
        if(retry_after != NULL)
            std::this_thread::sleep_for(std::chrono::seconds(std::stol(retry_after->value)));
        else
            std::this_thread::sleep_for(std::chrono::seconds(3));
        this->dataStream.open(this->saveLoc);
        curl_easy_setopt(this->curl, CURLOPT_FILE, &this->dataStream);

        cc = curl_easy_perform(this->curl);
        curl_easy_getinfo(this->curl, CURLINFO_RESPONSE_CODE, &http_code);
    }
    return cc;
}
void DownloadLibrary::CurlRequest::setUserAgent(std::string agent){
    this->usrAgent =agent;
    curl_easy_setopt(curl, CURLOPT_USERAGENT, agent.c_str());
}
DownloadLibrary::CurlRequest::~CurlRequest(){
    if(this->curl != nullptr) {
            curl_easy_cleanup(curl);
        }
}
int DownloadLibrary::CurlRequest::progress_callback(void *clientp , curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t  ulnow){
    struct progress_data* data = reinterpret_cast<struct progress_data *>(clientp);
    if(data->stop_switch==1){
        data->stop_switch =0;
        return 1;
    }
    data->total_download= dlnow;
    return 0;
}
int DownloadLibrary::CurlRequest::getDownloaded(){
    return this->prgData.total_download;
}
void DownloadLibrary::CurlRequest::stop(){
    this->prgData.stop_switch=1;
}
size_t DownloadLibrary::CurlRequest::write_function(char * data , size_t size , size_t nmemb, void * clientp){
    std::fstream *fs = reinterpret_cast<std::fstream*>(clientp);
    fs->write(data, size*nmemb);
    fs->flush();
    return nmemb;
}
void DownloadLibrary::CurlRequest::deleteF(){
    std::filesystem::remove(this->saveLoc);
}
std::atomic<long>* DownloadLibrary::CurlRequest::pGetDownloaded(){
    return &this->prgData.total_download;
}
