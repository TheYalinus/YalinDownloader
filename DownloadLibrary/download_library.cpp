#include "download_library.hpp"
#include <cstddef>
#include <curl/curl.h>
#include <curl/easy.h>
#include <exception>
#include <stdexcept>
#include <string>
#include <utility>
std::string DownloadLibrary::RangeType::get_range(){
    if(!this->first.empty() && !this->second.empty())
        return (this->first + "-" + this->second + '\0');
    else if(this->first.empty() && !this->second.empty())
        return ("-" + this->second + '\0');
    if(!this->first.empty()&& this->second.empty())
        return (this->first + "-" + '\0');
    if(this->first.empty() && this->second.empty())
        return "";
    return "";
}
DownloadLibrary::RangeType::RangeType(std::string a, std::string b):
std::pair<std::string,std::string>(a,b){

}
DownloadLibrary::RangeType::RangeType(std::string a):
std::pair<std::string , std::string>(a.substr(0,a.find("-")),a.substr(a.find("-")+1,a.size()-a.find("-")+1))
{

}
DownloadLibrary::TaskException::TaskException(CURLcode code ,std::string message):
message(message),ccode(code){
    if(message.empty()){
        if(code != 0){
        if (code==3)
            this->message="Malformatted url";
        else if(code==5)
            this->message="The given proxy cannot be resolved";
        else if(code==6)
            this->message="The given resolve host cannot be resolved";
        else if(code==7)
            this->message="Failed to connect proxy or host";
        else if(code==33)
            this->message="A problem occurred somewhere in the SSL/TLS handshake";
        else if(code==47)
            this->message="Too many redirects";
        else if(code==55)
            this->message="Failed sending network data";
        else if(code==56)
            this->message="Failed receiving network data(Maybe server does not supporting particular requests)";
        else if(code == 67)
            this->message="Login denied";
    }
    }
    }
