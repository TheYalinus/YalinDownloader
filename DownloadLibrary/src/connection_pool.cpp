#include "connection_pool.hpp"
#include "curl_wrapper.hpp"
#include "download_library.hpp"
#include <cstddef>
#include <curl/curl.h>
#include <curl/easy.h>
#include <curl/system.h>
#include <map>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <tuple>

DownloadLibrary::ConnectionPool::ConnectionPool(int n)
:curlConnections(CurlWrapperMapType {}),
curlShare()
{
    curlShare.setDNSShare();
    for(int i =0 ; i<n ; i++){
        std::cout<<"Connection a"<<std::endl;
        curlConnections.emplace(i, std::make_shared<CurlWrapper>());
        curl_easy_setopt(curlConnections[i]->getRawCurl(), CURLOPT_TCP_KEEPALIVE, 1L);
        curlConnections[i]->setShareHandle(this->curlShare.getSharePtr());
    }


}
DownloadLibrary::CurlWrapperPairType DownloadLibrary::ConnectionPool::getConnection(){
    for(auto a = curlConnections.begin() ; a!=curlConnections.end() ; a++){
        if(a->second.use_count() == 1)
            return *a;
    }
    {
        std::unique_lock<std::mutex> lk(cv_m);
        cv.wait(lk);
    }

    for(auto a = curlConnections.begin() ; a!=curlConnections.end() ; a++){
        if(a->second.use_count() == 1)
            return *a;
    }
    return {-1 , std::make_shared<CurlWrapper>()};
}
void DownloadLibrary::ConnectionPool::giveConnection(int i){
    if(this->curlConnections.find(i)->second.use_count() != 1){
        throw std::runtime_error("Shared pointer should be reset before returning it");
    }
    {

    std::unique_lock<std::mutex> lck(cv_m);
    cv.notify_one();
    }
}
DownloadLibrary::factory_data DownloadLibrary::ConnectionPool::initalizeConnections(std::string url, std::string user_agent , std::string dns, bool follow_redirects){
    factory_data result;
    //tod : use a share interface and write a wrapper for it
    for(auto &n : this->curlConnections){
        n.second->setUrl(url);

        if(!dns.empty())
            n.second->setDns(dns);
        if(!user_agent.empty())
            n.second->setUsrAgent(user_agent);
        if(follow_redirects)
            n.second->setFollowRedirects();
        //curl_easy_setopt(n.second->getRawCurl(), CURLOPT_WRITEFUNCTION, DownloadLibrary::ConnectionPool::dumm_write_callback);
        n.second->setHeaderOnly();
        std::cout<<"debug"<<std::endl;
        if(auto exec= n.second->executeCurl(); exec.second == 206) // important !!! fallback for servers who rejects head requests
            throw std::runtime_error("Error when initalizing connections"); //todo: write a fallback for this
    }
    result.total_size= this->curlConnections.at(0)->getTotalSize();
    result.efct_url = this->curlConnections.at(0)->getEffectiveUrl();
    result.cnt_type = this->curlConnections.at(0)->getHeader("Content-Type").value;
    auto effective_url_buff= this->curlConnections.at(0)->getEffectiveUrl();
    result.props={
        effective_url_buff.substr(effective_url_buff.find_last_of(".")+1),
        effective_url_buff.substr(effective_url_buff.find_last_of("/")+1,(effective_url_buff.find('?')-(effective_url_buff.find_last_of("/")))-1)

    };

    return result;
}
size_t DownloadLibrary::ConnectionPool::dumm_write_callback(char *ptr, size_t size, size_t nmemb, void *userdata){

    return -1;
}
