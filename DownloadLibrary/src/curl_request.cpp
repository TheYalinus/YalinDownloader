#include "download_library.hpp"
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
url(url), save_loc(save_loc), header_only(header_only),curl(curl_easy_init()),progress_data{0,0},stream(std::fstream()){
        curl_easy_setopt(this->curl, CURLOPT_XFERINFODATA, reinterpret_cast<void *>(&this->progress_data));
        curl_easy_setopt(this->curl, CURLOPT_NOPROGRESS, 0L);
        curl_easy_setopt(this->curl, CURLOPT_XFERINFOFUNCTION, progress_callback);
        curl_easy_setopt(this->curl, CURLOPT_WRITEFUNCTION, write_function);

        if(follow_redirects)

            curl_easy_setopt(this->curl, CURLOPT_FOLLOWLOCATION, 1L);
        if(user_agent != " "){
            this->user_agent = user_agent;
            curl_easy_setopt(this->curl, CURLOPT_USERAGENT, user_agent.c_str());
        }
        if(!std::filesystem::exists(this->save_loc))
        {std::ofstream a(this->save_loc); a.close();}

        this->stream.open(this->save_loc, std::ios::app);
        std::cout<<"Curl Request Debug 2: "<<url<< this->stream.is_open() <<std::endl;
        /*this->file = fopen(this->save_loc.c_str(), "a+");
        if(this->file == nullptr){
            std::cout<<"The file cannot be opened or does not exists"<<std::endl;
            std::ofstream x (this->save_loc);
            this->file = fopen(this->save_loc.c_str(), "w+");
            }*/
        curl_easy_setopt(this->curl, CURLOPT_FILE, &this->stream);

        if(range.first != "ignore"){

            curl_easy_setopt(this->curl,  CURLOPT_RANGE, range.get_range().c_str());
        }
        if (!dns.empty())
            curl_easy_setopt(this->curl, CURLOPT_DNS_SERVERS, dns.c_str());

        curl_easy_setopt(this->curl, CURLOPT_URL, this->url.c_str());

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
        std::filesystem::remove(std::filesystem::path(this->save_loc));
        this->stream.close();
        this->stream.clear();
        std::ofstream a(this->save_loc);
        a.close();
        curl_easy_header(curl, "Retry-After", 0, CURLH_HEADER, -1, &retry_after);
        if(retry_after != NULL)
            std::this_thread::sleep_for(std::chrono::seconds(std::stol(retry_after->value)));
        else
            std::this_thread::sleep_for(std::chrono::seconds(3));
        this->stream.open(this->save_loc);
        curl_easy_setopt(this->curl, CURLOPT_FILE, &this->stream);

        cc = curl_easy_perform(this->curl);
        curl_easy_getinfo(this->curl, CURLINFO_RESPONSE_CODE, &http_code);
    }
    return cc;
}
void DownloadLibrary::CurlRequest::setUserAgent(std::string agent){
    this->user_agent =agent;
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
int DownloadLibrary::CurlRequest::downloaded(){
    return this->progress_data.total_download;
}
void DownloadLibrary::CurlRequest::stop(){
    this->progress_data.stop_switch=1;
}
size_t DownloadLibrary::CurlRequest::write_function(char * data , size_t size , size_t nmemb, void * clientp){
    std::fstream *fs = reinterpret_cast<std::fstream*>(clientp);
    fs->write(data, size*nmemb);
    fs->flush();
    return nmemb;
}
void DownloadLibrary::CurlRequest::deletef(){
    std::filesystem::remove(this->save_loc);
}
std::atomic<long>* DownloadLibrary::CurlRequest::downloaded_ptr(){
    return &this->progress_data.total_download;
}
