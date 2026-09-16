#include "share_wrapper.hpp"
#include <curl/curl.h>
#include <stdexcept>
DownloadLibrary::ShareWrapper::ShareWrapper():shareHandle(curl_share_init(),curl_share_cleanup){

}
void DownloadLibrary::ShareWrapper::setDNSShare(bool option){
    if(option){
        auto sh = curl_share_setopt(this->shareHandle.get(), CURLSHOPT_SHARE, CURL_LOCK_DATA_DNS);
        if(sh)
            throw std::runtime_error(curl_share_strerror(sh));
    }
    else{
        auto sh = curl_share_setopt(this->shareHandle.get(), CURLSHOPT_UNSHARE, CURL_LOCK_DATA_DNS);
        if(sh)
            throw std::runtime_error(curl_share_strerror(sh));
    }

}
CURLSH * DownloadLibrary::ShareWrapper::getSharePtr(){
    return this->shareHandle.get();
}
