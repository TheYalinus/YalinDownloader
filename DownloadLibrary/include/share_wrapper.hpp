#ifndef  SHARE_WRAPPER_HPP
#define  SHARE_WRAPPER_HPP
#include "download_library.hpp"
#include <curl/curl.h>
#include <memory>
namespace DownloadLibrary{
    using uniqueShareHandleType = std::unique_ptr<CURLSH , decltype(&curl_share_cleanup)>;
    class ShareWrapper{
        public:
            ShareWrapper();
        private:
            uniqueShareHandleType shareHandle;
    };
}
#endif
