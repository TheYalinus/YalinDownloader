#include "download_library.hpp"
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
std::pair<std::string , std::string>(a.substr(0,a.find("-")-1),a.substr(a.find("-")+1))
{

}
