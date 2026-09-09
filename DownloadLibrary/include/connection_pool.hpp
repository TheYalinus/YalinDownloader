#ifndef CONNECTION_POOL_HPP
#define CONNECTION_POOL_HPP
#include <condition_variable>
#include <map>
#include <curl/curl.h>
#include <memory>
#include <tuple>
#include "download_library.hpp"
#include "curl_wrapper.hpp"
#include <curl/curl.h>
#include <curl/easy.h>
#include <curl/header.h>
#include <curl/system.h>
namespace DownloadLibrary {
    using CurlWrapperType= std::shared_ptr<CurlWrapper>;
    using CurlWrapperMapType= std::map<int, CurlWrapperType>;
    using CurlWrapperPairType= std::pair<int, std::shared_ptr<CurlWrapper>>;
    class ConnectionPool{
        public:
            ConnectionPool(int i);
            CurlWrapperPairType getConnection();
            void giveConnection(int);
        private:
            std::mutex cv_m;
            std::condition_variable cv;
            CurlWrapperMapType curlConnections;

    };
    using sharedPoolType = std::shared_ptr<ConnectionPool>;
};

#endif
