#include "download_library.hpp"
#include <curl/curl.h>
#include <curl/easy.h>
#include <curl/system.h>
#include <sstream>
#include <stdexcept>


DownloadLibrary::CurlRequest::CurlRequest(std::string url, std::string save_loc , bool header_only, RangeType range,  std::string user_agent,bool follow_redirects):
url(url), save_loc(save_loc), header_only(header_only),curl(curl_easy_init()),progress_data{0,0}{
        curl_easy_setopt(this->curl, CURLOPT_XFERINFODATA, reinterpret_cast<void *>(&this->progress_data));
        curl_easy_setopt(this->curl, CURLOPT_NOPROGRESS, 0L);
        curl_easy_setopt(this->curl, CURLOPT_XFERINFOFUNCTION, progress_callback);
        if(follow_redirects)
            curl_easy_setopt(this->curl, CURLOPT_FOLLOWLOCATION, 1L);
        if(user_agent != " "){
            this->user_agent = user_agent;
            curl_easy_setopt(this->curl, CURLOPT_USERAGENT, user_agent.c_str());
        }
        this->file = fopen(this->save_loc.c_str(), "w+");
        if(this->file == nullptr)
            throw std::runtime_error("The file cannot be opened or does not exists");
        curl_easy_setopt(this->curl, CURLOPT_FILE, this->file);
        if(range.first != "ignore"){

            curl_easy_setopt(this->curl,  CURLOPT_RANGE, range.get_range().c_str());
        }
        curl_easy_setopt(this->curl, CURLOPT_URL, this->url.c_str());

}
CURLcode DownloadLibrary::CurlRequest::curlPerform(){
    return curl_easy_perform(this->curl);
}
void DownloadLibrary::CurlRequest::setUserAgent(std::string agent){
    this->user_agent =agent;
    curl_easy_setopt(curl, CURLOPT_USERAGENT, agent.c_str());
}
DownloadLibrary::CurlRequest::~CurlRequest(){
    if(this->file != nullptr)
        fclose(file);
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
