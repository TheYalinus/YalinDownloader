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

//May be this class name converted to more sensible name like Part
DownloadLibrary::CurlRequest::CurlRequest(sharedPoolType connectionPool , std::string url, std::string save_loc , bool header_only, RangeType range,  std::string user_agent, std::string dns , bool follow_redirects):
Url(url), saveLoc(save_loc), isHdrOnly(header_only),prgData{0,0},dataStream(std::fstream()),followRedirects(follow_redirects),usrAgent(user_agent), range(range), connectionPool(connectionPool){
        if(!std::filesystem::exists(this->saveLoc))
        {std::ofstream a(this->saveLoc); a.close();}
        this->dataStream.open(this->saveLoc, std::ios::app);
        /*this->file = fopen(this->save_loc.c_str(), "a+");
        if(this->file == nullptr){
            std::cout<<"The file cannot be opened or does not exists"<<std::endl;
            std::ofstream x (this->save_loc);
            this->file = fopen(this->save_loc.c_str(), "w+");
            }*/





}
//todo:?maybe a mutex lock mechanic for starting connections not at same time, preventing 429
CURLcode DownloadLibrary::CurlRequest::curlPerform(){

    auto [i, curlw]= this->connectionPool->getConnection();

    curl_easy_setopt(curlw->getRawCurl(), CURLOPT_XFERINFODATA, reinterpret_cast<void *>(&this->prgData));
    curlw->setProgress();
    curl_easy_setopt(curlw->getRawCurl(), CURLOPT_XFERINFOFUNCTION, progress_callback);
    curl_easy_setopt(curlw->getRawCurl(), CURLOPT_WRITEFUNCTION, write_function);
    if(this->followRedirects)
        curlw->setFollowRedirects();
    if(this->usrAgent != " "){
        curlw->setUsrAgent(this->usrAgent);
    }
    curl_easy_setopt(curlw->getRawCurl(), CURLOPT_FILE, &this->dataStream);
    curlw->setRange(this->range);
    if (!this->dns.empty())
        curlw->setDns(this->dns);

    curlw->setUrl(this->Url);
    curlw->setHeaderOnly(false);
    std::cout<<"curlPerform"<<std::endl;
    CURLcode cc;
    long http_code;
    auto exec =curlw->executeCurl();
    cc= exec.first;
    http_code= exec.second;
    std::cout<<cc<<"-"<<http_code<<std::endl;
    while(http_code == 429){
        curl_off_t retry=0;
        std::cout<<"error 429"<<std::endl;
        struct curl_header *retry_after;
        std::filesystem::remove(std::filesystem::path(this->saveLoc));
        this->dataStream.close();
        this->dataStream.clear();
        std::ofstream a(this->saveLoc);
        a.close();
        curl_easy_getinfo(curlw->getRawCurl(), CURLINFO_RETRY_AFTER, &retry);
        if(retry_after != 0)
            std::this_thread::sleep_for(std::chrono::seconds(retry));
        else
            std::this_thread::sleep_for(std::chrono::seconds(4));

        this->dataStream.open(this->saveLoc);
        curl_easy_setopt(curlw->getRawCurl(), CURLOPT_FILE, &this->dataStream);
        auto exec =curlw->executeCurl();
        cc= exec.first;
        http_code= exec.second;
    }
    //todo : add a fallback for other errors
    curlw.reset();
    connectionPool->giveConnection(i);
    return cc;
}

DownloadLibrary::CurlRequest::~CurlRequest(){

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
