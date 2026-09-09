#include "curl_wrapper.hpp"
#include "download_library.hpp"
#include <curl/curl.h>
#include <curl/easy.h>

DownloadLibrary::CurlWrapper::CurlWrapper():
curl_handle(curl_easy_init(), &curl_easy_cleanup){

}

void DownloadLibrary::CurlWrapper::setUrl(std::string url){
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
