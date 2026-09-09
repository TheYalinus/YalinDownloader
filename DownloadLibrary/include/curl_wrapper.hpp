#ifndef  CURL_WRAPPER_HPP
#define CURL_WRAPPER_HPP
#include <curl/curl.h>
#include <curl/easy.h>
#include <memory>
#include "download_library.hpp"
namespace DownloadLibrary {
    using uniqueCurlHandleType = std::unique_ptr<CURL, decltype(&curl_easy_cleanup)> ;
    class CurlWrapper{
        public:
            CurlWrapper();
            void setUrl(std::string);
            void setDns(std::string);
            void setProxy(std::string);
            void setRange(RangeType);
            void setHeaderOnly(bool=true);
            void setFollowRedirects(bool=true);
            void resetAttributes();
            CURL * getRawCurl();
            std::pair <CURLcode,long> executeCurl();
        private:
            uniqueCurlHandleType curl_handle;
    };
}
#endif
