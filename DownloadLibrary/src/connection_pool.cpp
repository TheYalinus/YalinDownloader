#include "connection_pool.hpp"
#include "curl_wrapper.hpp"
#include <curl/curl.h>
#include <map>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <tuple>

DownloadLibrary::ConnectionPool::ConnectionPool(int n)
:curlConnections(CurlWrapperMapType {})
{
    for(int i =0 ; i<n ; i++){
        curlConnections.emplace(i, std::make_shared<CurlWrapper>());
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
