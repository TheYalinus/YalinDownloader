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
std::pair<std::string , std::string>(a.substr(0,a.find("-")-1),a.substr(a.find("-")+1))
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

DownloadLibrary::DownloadTask* DownloadLibrary::DownloadFactory::createTask(std::string url, std::string task_location, int part_count,  bool follow_redirects,file_properties props , std::string user_agent, std::string dns){
        factory_data result;

        CURL * curl = curl_easy_init();
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        if(!dns.empty())
            curl_easy_setopt(curl, CURLOPT_DNS_SERVERS, dns.c_str());
        if(!user_agent.empty())
            curl_easy_setopt(curl, CURLOPT_USERAGENT, user_agent.c_str());
        if(follow_redirects)
            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_NOBODY, 1L);
        CURLcode  result_code = curl_easy_perform(curl);
        if(result_code ==0){
            result = gather_head(curl);
            curl_easy_cleanup(curl);
            return new DownloadTaskMultiple(HEADER_ACCEPT,result,url,task_location,part_count,user_agent,dns,follow_redirects,props);
        }
        else if(result_code ==56){
            std::cout<<result_code<<std::endl;
            result = gather_get(curl);
            curl_easy_cleanup(curl);
            return new DownloadTaskMultiple(HEADER_REJECT,result, url, task_location, part_count, user_agent, dns, follow_redirects, props );
            //pass the resulst to class
        }
        else{
            throw  std::runtime_error("Problem while fetching data"+std::to_string(result_code));

        }
    }
    /*int DownloadLibrary::DownloadFactory::dumm_progress_callback(void *clientp, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow){
        if(dlnow>=1){
            return 1;
        }
        return 0;
        }*/
    size_t DownloadLibrary::DownloadFactory::dumm_write_callback(char *ptr, size_t size, size_t nmemb, void *userdata){
     return -1;
    }
    DownloadLibrary::factory_data DownloadLibrary::DownloadFactory::gather_get(CURL * curl){
        factory_data result;
        CURLcode result_code;
        std::string ext_buffer;
        curl_off_t total_size_buffer=0;
        char * effective_url_buff_c;
        struct curl_header *type;
        std::string full_file_name;
        std::string extension;
        curl_easy_setopt(curl, CURLOPT_NOBODY, 0L);
        curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
        /*curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, dumm_progress_callback);
            curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);*/
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, dumm_write_callback);
        result_code = curl_easy_perform(curl);
        std::cout<<result_code <<std::endl;
        if(result_code != CURLE_OK && result_code !=CURLE_WRITE_ERROR){
            //Single parted fallback

            curl_easy_cleanup(curl);
            throw std::runtime_error("not ok");
        }

        curl_easy_getinfo(curl, CURLINFO_EFFECTIVE_URL, &effective_url_buff_c);
        curl_easy_getinfo(curl, CURLINFO_CONTENT_LENGTH_DOWNLOAD_T, &total_size_buffer);
        std::cout<<total_size_buffer<<std::endl;
        curl_easy_header(curl, "Content-Type", 0, CURLH_HEADER, -1, &type);
        if(effective_url_buff_c == NULL){
            std::cerr<<"Libcurl has not given the effective url, so the name cannot be determined";
            extension="";
            //random name gen
            full_file_name  = "Unknown File";
        }
        else{
            std::string effective_url_buff(effective_url_buff_c);
            full_file_name = effective_url_buff.substr(effective_url_buff.find_last_of("/")+1,(effective_url_buff.find('?')-(effective_url_buff.find_last_of("/")))-1);
            extension = full_file_name.substr(full_file_name.find_last_of(".")+1);
        }
        curl_easy_cleanup(curl);std::cout<<result_code<<std::endl;
        result.props.file_name= full_file_name;
        result.props.file_extension= extension;
        result.cnt_type = type->value;
        result.efct_url= effective_url_buff_c;
        result.total_size= total_size_buffer;
        return result;
    }
    DownloadLibrary::factory_data DownloadLibrary::DownloadFactory::gather_head(CURL * curl){
        factory_data result;
        curl_off_t total_size_buffer=0;
        std::string ext_buffer;
        char * effective_url_buff_c;
        struct curl_header *type;
        std::string full_file_name;
        std::string extension;
        curl_easy_getinfo(curl, CURLINFO_CONTENT_LENGTH_DOWNLOAD_T, &total_size_buffer);
        curl_easy_getinfo(curl, CURLINFO_EFFECTIVE_URL, &effective_url_buff_c);
        curl_easy_header(curl, "Content-Type", 0, CURLH_HEADER, -1, &type);
        if(effective_url_buff_c == NULL){
            std::cerr<<"Libcurl has not given the effective url, so the name cannot be determined";
            extension="";
            //random name gen
                full_file_name  = "Unknown File";
            }
        else{
                std::string effective_url_buff(effective_url_buff_c);
                full_file_name = effective_url_buff.substr(effective_url_buff.find_last_of("/")+1,(effective_url_buff.find('?')-(effective_url_buff.find_last_of("/")))-1);
                extension = full_file_name.substr(full_file_name.find_last_of(".")+1);
            }

        result.props.file_name= full_file_name;
        result.props.file_extension= extension;
        result.cnt_type = type->value;
        result.efct_url= effective_url_buff_c;
        result.total_size= total_size_buffer;
        return result;
    }
