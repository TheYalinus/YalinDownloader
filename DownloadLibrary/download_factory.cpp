#include "download_factory.hpp"
#include "download_library.hpp"
#include <curl/easy.h>
#include <stdexcept>
#include <string>
DownloadLibrary::DownloadTask* DownloadLibrary::DownloadFactory::createTask(std::string url, std::string task_location, int part_count, std::string user_agent,  bool follow_redirects,file_properties props , std::string dns){
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
        if(result_code ==CURLE_OK){
            result = gather_head(curl);
            curl_easy_cleanup(curl);
            return new DownloadTaskMultiple(HEADER_ACCEPT,result,url,task_location,part_count,user_agent,dns,follow_redirects,props);
        }
        else if(result_code ==56){
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
        char * effective_url_buff_c =NULL;
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
        char * effective_url_buff_c=NULL;
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
    DownloadLibrary::DownloadTask * DownloadLibrary::DownloadFactory::createTask(std::string task_location, std::string user_agent, std::string dns, bool follow_redirects){
        if(!std::filesystem::exists(task_location))
            throw std::runtime_error("Directory does not exists");
        else if (!std::filesystem::is_directory(task_location))
            throw std::runtime_error("Specified path is not directory.");
        else if(std::filesystem::is_empty(task_location))
            throw std::runtime_error("Directory is empty.");

        factory_data fdata;
        auto cfg_path= std::filesystem::path( task_location ) / "download.json";

        if(!std::filesystem::exists(cfg_path))
            throw std::runtime_error("There is no download.json file in specified directory");

        std::fstream cfgstream(cfg_path);
        json cfg_data=  json::parse(cfgstream);//json::parse_error should be catched by class creator
        cfgstream.close();
        std::string url = cfg_data["url"];
        CURL * curl = curl_easy_init();
        std::cout<<url<<std::endl;
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

        if(!dns.empty())
            curl_easy_setopt(curl, CURLOPT_DNS_SERVERS, dns.c_str());
        if(!user_agent.empty())
            curl_easy_setopt(curl, CURLOPT_USERAGENT, user_agent.c_str());
        if(follow_redirects)
            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);




        if(cfg_data["header-flag"]==0){
            curl_easy_setopt(curl, CURLOPT_NOBODY, 1L);
            curl_easy_perform(curl);
            fdata = gather_head(curl);
            std::cout<<"Debug #4-2"<<std::endl;
        }
        else {
            fdata =gather_get(curl);
        }
        std::string cnttype= cfg_data.at("cnt-type");
        int totalsize= cfg_data.at("total_size");
        if(cnttype !=  fdata.cnt_type || totalsize != static_cast<int>(fdata.total_size))
            throw std::runtime_error("Broken link");
        curl_easy_cleanup(curl);
        return new DownloadLibrary::DownloadTaskMultiple(task_location, user_agent, dns,true);
    }
