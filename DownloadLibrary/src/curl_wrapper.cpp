#include "curl_wrapper.hpp"
#include "download_library.hpp"
#include <curl/curl.h>
#include <curl/easy.h>
#include <curl/system.h>

DownloadLibrary::CurlWrapper::CurlWrapper():
curl_handle(curl_easy_init(), &curl_easy_cleanup){
    curl_easy_setopt(this->curl_handle.get(),CURLOPT_VERBOSE,1L);
}

void DownloadLibrary::CurlWrapper::setUrl(std::string url){
    //TODO : dont set url if already set to same
    curl_easy_setopt(this->curl_handle.get(),CURLOPT_URL,url.c_str());
}
void DownloadLibrary::CurlWrapper::setDns(std::string dns){
    curl_easy_setopt(this->curl_handle.get(), CURLOPT_DNS_SERVERS, dns.c_str());
}
void DownloadLibrary::CurlWrapper::setProxy(std::string proxy){
    //implement proxy logic

}
void DownloadLibrary::CurlWrapper::setRange(DownloadLibrary::RangeType range){
    curl_easy_setopt(this->curl_handle.get(), CURLOPT_RANGE, range.get_range().c_str());
}

void DownloadLibrary::CurlWrapper::setHeaderOnly(bool option){
    if (option)
        curl_easy_setopt(this->curl_handle.get(), CURLOPT_NOBODY, 1L);
    else
        curl_easy_setopt(this->curl_handle.get(), CURLOPT_NOBODY, 0L);
}
void DownloadLibrary::CurlWrapper::resetAttributes(){
    setHeaderOnly(false);
    setFollowRedirects();
    setRange({"",""});
}
CURL * DownloadLibrary::CurlWrapper::getRawCurl(){
    return this->curl_handle.get();
}
void DownloadLibrary::CurlWrapper::setFollowRedirects(bool option){
    if(option)
        curl_easy_setopt(this->curl_handle.get(), CURLOPT_FOLLOWLOCATION, 1L);
    else
        curl_easy_setopt(this->curl_handle.get(), CURLOPT_FOLLOWLOCATION, 0L);

}
std::pair <CURLcode,long> DownloadLibrary::CurlWrapper::executeCurl(){
    long http_code = 0;
    CURLcode code =curl_easy_perform(this->curl_handle.get());
    curl_easy_getinfo(this->curl_handle.get(), CURLINFO_RESPONSE_CODE, &http_code);
    return {code , http_code};
}
void DownloadLibrary::CurlWrapper::setProgress(bool option){
    if(option)
        curl_easy_setopt(this->curl_handle.get(), CURLOPT_NOPROGRESS, 0L);
    else
        curl_easy_setopt(this->curl_handle.get(), CURLOPT_NOPROGRESS, 1L);
}
void DownloadLibrary::CurlWrapper::setShareHandle(CURLSH * share_handle){
    curl_easy_setopt(this->getRawCurl(), CURLOPT_SHARE, share_handle);
}
void DownloadLibrary::CurlWrapper::setUsrAgent(std::string usrAgent){
    curl_easy_setopt(this->curl_handle.get(), CURLOPT_USERAGENT, usrAgent.c_str());
}
std::string DownloadLibrary::CurlWrapper::getEffectiveUrl(){
    char * effective_url_buff_c;
    curl_easy_getinfo(this->curl_handle.get(), CURLINFO_EFFECTIVE_URL, &effective_url_buff_c);
    if(effective_url_buff_c != NULL){
        return std::string{effective_url_buff_c};
    }
    else {
        return std::string{"unknown"};
    }
}
curl_off_t DownloadLibrary::CurlWrapper::getTotalSize(){
    curl_off_t total_size_buffer=0;
    curl_easy_getinfo(this->curl_handle.get(), CURLINFO_CONTENT_LENGTH_DOWNLOAD_T, &total_size_buffer);
    return total_size_buffer;
}
struct curl_header DownloadLibrary::CurlWrapper::getHeader(std::string value){
    struct curl_header *type;
    curl_easy_header(this->curl_handle.get(), value.c_str(), 0, CURLH_HEADER, -1, &type);
    return *type;
}
