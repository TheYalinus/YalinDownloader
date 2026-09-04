#ifndef CURL_REQUEST_HPP
#define CURL_REQUEST_HPP
#include "download_library.hpp"
namespace DownloadLibrary {
    class CurlRequest{
        private:
            std::string Url;
            std::string dwnRange;
            std::string saveLoc;
            bool isHdrOnly;
            FILE* file;
            CURL* curl;
            std::string usrAgent;
            struct progress_data prgData;
            std::fstream dataStream;
        public:
            CurlRequest(std::string url,std::string saveLoc,bool isHdrOnly,RangeType dwnRange = {"",""},  std::string usrAgent =" ", std::string dns ="", bool flwRedirect=true);
            CURLcode curlPerform();
            void setUserAgent(std::string agent);
            static int progress_callback(void *clientp,
                                  curl_off_t  dltotal,
                                  curl_off_t  dlnow,
                                  curl_off_t  ultotal,
                                  curl_off_t  ulnow);
            static size_t write_function(char * data , size_t size , size_t nmemb, void * clientp);
            int getDownloaded();
            std::atomic<long>* pGetDownloaded();
            void stop();
            void deleteF();
            ~CurlRequest();

    };
}
#endif
