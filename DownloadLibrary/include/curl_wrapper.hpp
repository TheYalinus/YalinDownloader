#ifndef  CURL_WRAPPER_HPP
#define CURL_WRAPPER_HPP
#include <curl/curl.h>
#include <curl/easy.h>
#include <curl/header.h>
#include <curl/system.h>
#include <memory>
#include "download_library.hpp"
//file wrapper that uses the same at curl_request, it should be moved here
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
            void setProgress(bool=true);
            void setUsrAgent(std::string);
            void setShareHandle(CURLSH *);
            void resetAttributes();
            CURL * getRawCurl();
            std::string getEffectiveUrl();
            curl_off_t getTotalSize();
            struct curl_header getHeader(std::string value);
            std::pair <CURLcode,long> executeCurl();
        private:
            uniqueCurlHandleType curl_handle;
    };
}
#endif
