#include "share_wrapper.hpp"
#include <curl/curl.h>
DownloadLibrary::ShareWrapper::ShareWrapper():shareHandle(curl_share_init(),curl_share_cleanup){

}
